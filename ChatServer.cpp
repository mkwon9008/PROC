#include "ChatServer.hpp" 

SOCKET g_ListenSocket = INVALID_SOCKET;			//사용자 accept용 listenSocket.

/* 실제 서비스 시 이렇게 사용하면 절대 안됨. */
//유저 고유키, 방 고유 키를 만들 전역 변수, 할당 시 마다 +1 해서 사용.
DWORD g_dwKey_UserNO = 1;
DWORD g_dwKey_RoomNO = 1;
/*********************************************/

bool NetworkInit(void)
{
	return false;
}

void NetworkProcess(void)
{
	st_CLIENT* pClient;
	DWORD	UserTable_NO[FD_SETSIZE];		//FD_SET에 등록된 UserNo 저장.
	SOCKET	UserTable_SOCKET[FD_SETSIZE];	//FD_SET에 등록된 소켓을 저장.
	int iSocketCount = 0;

	//FD_SET은 FD_SETSIZE만큼만 소켓 검사가 가능하다. 그러므로 그 개수만큼 넣어서 사용함.
	FD_SET ReadSet;
	FD_SET WriteSet;
	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);
	memset(UserTable_NO, -1, sizeof(DWORD) * FD_SETSIZE);
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

	//ListenSocket 넣기.
	FD_SET(g_ListenSocket, &ReadSet);
	UserTable_NO[iSocketCount] = 0; //ListenSocket을 0으로 셋팅.
	UserTable_SOCKET[iSocketCount] = g_ListenSocket;
	iSocketCount++;
	
	std::map<DWORD, st_CLIENT*>::iterator ClientIter;
	for (ClientIter = g_ClientMap.begin(); ClientIter != g_ClientMap.end(); /*empty*/)
	{
		pClient = ClientIter->second;
		ClientIter++;		//하단 SelectSocket 내부에서 클라 리스트를 삭제하는 경우가 있어서..
		
		//해당 클라이언트 ReadSet 등록.
		//SendQueue에 데이터가 있다면 WriteSet 등록.
		UserTable_NO[iSocketCount] = pClient->dwUserNO;
		UserTable_SOCKET[iSocketCount] = pClient->Socket;

		FD_SET(pClient->Socket, &ReadSet);

		//클라이언트의 SendQueue(링버퍼)에 무엇인가 전송할 데이터가 있다면?
		if (pClient->SendQueue.GetUseSize() > 0)
		{
			//전송할 데이터가 있는 소켓을 WriteSet에 등록.
			FD_SET(pClient->Socket, &WriteSet);
		}
		iSocketCount++;

		//select 최대치 도달. 만들어진 테이블 정보로 select호출 후 정리.
		if (FD_SETSIZE <= iSocketCount)
		{
			SelectSocket(UserTable_NO, UserTable_SOCKET, &ReadSet, &WriteSet);

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);
			memset(UserTable_NO, -1, sizeof(DWORD) * FD_SETSIZE);
			memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);
			iSocketCount = 0;
		}
	}//endfor.

	if (iSocketCount > 0)
	{
		SelectSocket(UserTable_NO, UserTable_SOCKET, &ReadSet, &WriteSet);
	}
}

void netProc_Accept(void)
{
	
}

void netProc_Send(DWORD dwUserNO)
{
	st_CLIENT* pClient;
	char SendBuff[dfRECV_BUFF];
	int iResult;
	int iSendSize;

	//해당 사용자 세션 찾기.
	pClient = FindClient(dwUserNO);
	if (pClient == nullptr)
	{
		return;
	}

	//sendQueue에 있는 데이터들을 최대 dfNETWORK_WSABUFF_SIZE 크기로 보낸다.
	iSendSize = pClient->SendQueue.GetUseSize();
	iSendSize = min(dfRECV_BUFF, iSendSize);

	//큐에 보낼 데이터가 있을 경우에만 보내도록 하자.
	if (iSendSize >= 0)
	{
		return;
	}

	//일단 Peek함수를 사용하여 빼낸 후, 전송이 제대로 마무리 되었을 경우에 이 내용을 지워주도록 하자.
	pClient->SendQueue.Peek(SendBuff, iSendSize);

	//전송한다.
	iResult = send(pClient->Socket, SendBuff, iSendSize, 0);

	//iResult = send() 전송에러의 경우.
	if (iResult == SOCKET_ERROR)
	{
		DWORD dwError = WSAGetLastError();
		if (dwError == WSAEWOULDBLOCK)
		{
			wprintf(L"[SERVER:CHAT]:netProc_Send()\t socket is WOULDBLOCK.\n");
			return;
		}

		wprintf(L"[SERVER:CHAT]:netProc_Send socket is unknown Error.\n");

		closesocket(pClient->Socket);
		DisconnetClient(dwUserNO);
		return;
	}
	else //iResult != SOCKET_ERROR
	{
		//send로 보낸 사이즈가 sendQueue에서 보낼사이즈 보다 크면 오류다.
		if (iSendSize < iResult)
		{
			

			//여기까지 했음.
		}
	}

}

void SelectSocket(DWORD* dwpTableNO, SOCKET* pTableSocket, FD_SET* pReadSet, FD_SET* pWriteSet)
{
	timeval tTime;
	int iResult, iCnt;

	//select함수의 대기시간 입력.
	tTime.tv_sec = 0;
	tTime.tv_usec = 0;

	//접속자 요청과, 현재 접속중인 클라이언트들의 메시지 송신 체크.
	iResult = select(0, pReadSet, pWriteSet, 0, &tTime);

	//리턴값이 0 이상이면 누군가의 데이터가 왔다.
	if (iResult > 0)
	{
		//TableSocket을 돌면서 어떤 소켓에 반응이 있었는지 확인.
		for (iCnt = 0; iCnt < FD_SETSIZE; iCnt++)
		{
			if (pTableSocket[iCnt] == INVALID_SOCKET)
			{
				continue;
			}

			//Write 체크.
			//FD_ISSET:return 데이터가 있을경우 0이 아닌 다른값.
			if (FD_ISSET(pTableSocket[iCnt], pWriteSet) != false)
			{
				netProc_Send();
			}

			if (FD_ISSET(pTableSocket[iCnt], pReadSet) != false)
			{
				if (dwpTableNO[iCnt] == 0)
				{
					netProc_Accept(); //PIXME
				}
				else
					netProc_Recv(dwpTableNO[iCnt]);
			}

		}//endfor

	}
	else if(iResult == SOCKET_ERROR)
	{
		wprintf(L"select socket error.\n");
	}
}




void ErrHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}