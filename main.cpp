#include "ChatServer.hpp"

/*
#기본 프로토콜.

(                     HEADER                   )(  CONTENTS )
| PacketCode | CheckSum | MsgType | PayloadSize | *Payload* |
	1byte		1byte		2byte		2byte	   SizeByte

CheckSum : 각 MsgType, Payload의 각 바이트 더하기 % 256 (PayloadSize 는 미포함) (사용자에 의한 데이터 위.변조를 막기위해 사용)
Payload의 암호화에 성공했냐, 복호화에 성공했냐를 CheckSum 값으로 판단해야 한다.

바이트(BYTE) : unsigned char

*/


int main(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	if (NetworkInit() == true)
	{
		wprintf(L"[SERVER:CHAT]:Network_Initialize()\tNetwork initialize fail.\n");
		return 0;
	}

	while (true)
	{
		NetworkProcess();

		//특정 키 및 상황에 따른 종료처리.
		if (true)//체크...
		{
			closesocket(g_ListenSocket);
			break;
		}
	}

	//서버 마무리 종료작업.
	return 0;

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;

	int adrSz;
	int fdNum = 0, iCnt;

	TIMEVAL timeout;

	fd_set reads, cpyReads;

	//WSAStartup()
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrHandling("WSAStartup() Err.");
	printf("WSAStartup() OK.\n");

	//1. listen() 소켓 생성.
	//Declaration ServSock
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	
	//서버소켓 할당 에러체크.
	if (hServSock == INVALID_SOCKET)
		ErrHandling("socket() Error");
	printf("socket() OK.\n");

	//2. 구조체 생성.
	//ZeroMemory로 servAddr밀고, 인자 할당.
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(dfNETWORK_PORT);
	printf("SETservAddr OK.\n");


	//3. listen소켓에 구조체 적용해서 bind();
	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrHandling("bind() Err.");
	printf("bind() OK.\n");


	//4. binding 이 완료 된 hServSock 으로 listen() 시작.
	//listen(소켓디스크립터, 대기 메시지 큐의 갯수);
	if (listen(hServSock, 10) == SOCKET_ERROR)
		ErrHandling("listen() Err.");
	printf("listen() OK\n");

	//readset 초기화.
	FD_ZERO(&reads);
	//파일디스크립터에 readset 추가.
	FD_SET(hServSock, &reads);

	while (true)
	{
		cpyReads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if (fdNum == 0)
			continue;

		for (iCnt = 0; iCnt < reads.fd_count; iCnt++)
		{
			if (FD_ISSET(reads.fd_array[iCnt], &cpyReads))
			{
				adrSz = sizeof(clntAdr);
				hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
				
			}
		}

	}



	return 0;
}