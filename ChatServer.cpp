#include "ChatServer.hpp" 

SOCKET g_ListenSocket = INVALID_SOCKET;			//����� accept�� listenSocket.

/* ���� ���� �� �̷��� ����ϸ� ���� �ȵ�. */
//���� ����Ű, �� ���� Ű�� ���� ���� ����, �Ҵ� �� ���� +1 �ؼ� ���.
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
	DWORD	UserTable_NO[FD_SETSIZE];		//FD_SET�� ��ϵ� UserNo ����.
	SOCKET	UserTable_SOCKET[FD_SETSIZE];	//FD_SET�� ��ϵ� ������ ����.
	int iSocketCount = 0;

	//FD_SET�� FD_SETSIZE��ŭ�� ���� �˻簡 �����ϴ�. �׷��Ƿ� �� ������ŭ �־ �����.
	FD_SET ReadSet;
	FD_SET WriteSet;
	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);
	memset(UserTable_NO, -1, sizeof(DWORD) * FD_SETSIZE);
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

	//ListenSocket �ֱ�.
	FD_SET(g_ListenSocket, &ReadSet);
	UserTable_NO[iSocketCount] = 0; //ListenSocket�� 0���� ����.
	UserTable_SOCKET[iSocketCount] = g_ListenSocket;
	iSocketCount++;
	
	std::map<DWORD, st_CLIENT*>::iterator ClientIter;
	for (ClientIter = g_ClientMap.begin(); ClientIter != g_ClientMap.end(); /*empty*/)
	{
		pClient = ClientIter->second;
		ClientIter++;		//�ϴ� SelectSocket ���ο��� Ŭ�� ����Ʈ�� �����ϴ� ��찡 �־..
		
		//�ش� Ŭ���̾�Ʈ ReadSet ���.
		//SendQueue�� �����Ͱ� �ִٸ� WriteSet ���.
		UserTable_NO[iSocketCount] = pClient->dwUserNO;
		UserTable_SOCKET[iSocketCount] = pClient->Socket;

		FD_SET(pClient->Socket, &ReadSet);

		//Ŭ���̾�Ʈ�� SendQueue(������)�� �����ΰ� ������ �����Ͱ� �ִٸ�?
		if (pClient->SendQueue.GetUseSize() > 0)
		{
			//������ �����Ͱ� �ִ� ������ WriteSet�� ���.
			FD_SET(pClient->Socket, &WriteSet);
		}
		iSocketCount++;

		//select �ִ�ġ ����. ������� ���̺� ������ selectȣ�� �� ����.
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

	//�ش� ����� ���� ã��.
	pClient = FindClient(dwUserNO);
	if (pClient == nullptr)
	{
		return;
	}

	//sendQueue�� �ִ� �����͵��� �ִ� dfNETWORK_WSABUFF_SIZE ũ��� ������.
	iSendSize = pClient->SendQueue.GetUseSize();
	iSendSize = min(dfRECV_BUFF, iSendSize);

	//ť�� ���� �����Ͱ� ���� ��쿡�� �������� ����.
	if (iSendSize >= 0)
	{
		return;
	}

	//�ϴ� Peek�Լ��� ����Ͽ� ���� ��, ������ ����� ������ �Ǿ��� ��쿡 �� ������ �����ֵ��� ����.
	pClient->SendQueue.Peek(SendBuff, iSendSize);

	//�����Ѵ�.
	iResult = send(pClient->Socket, SendBuff, iSendSize, 0);

	//iResult = send() ���ۿ����� ���.
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
		//send�� ���� ����� sendQueue���� ���������� ���� ũ�� ������.
		if (iSendSize < iResult)
		{
			

			//������� ����.
		}
	}

}

void SelectSocket(DWORD* dwpTableNO, SOCKET* pTableSocket, FD_SET* pReadSet, FD_SET* pWriteSet)
{
	timeval tTime;
	int iResult, iCnt;

	//select�Լ��� ���ð� �Է�.
	tTime.tv_sec = 0;
	tTime.tv_usec = 0;

	//������ ��û��, ���� �������� Ŭ���̾�Ʈ���� �޽��� �۽� üũ.
	iResult = select(0, pReadSet, pWriteSet, 0, &tTime);

	//���ϰ��� 0 �̻��̸� �������� �����Ͱ� �Դ�.
	if (iResult > 0)
	{
		//TableSocket�� ���鼭 � ���Ͽ� ������ �־����� Ȯ��.
		for (iCnt = 0; iCnt < FD_SETSIZE; iCnt++)
		{
			if (pTableSocket[iCnt] == INVALID_SOCKET)
			{
				continue;
			}

			//Write üũ.
			//FD_ISSET:return �����Ͱ� ������� 0�� �ƴ� �ٸ���.
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