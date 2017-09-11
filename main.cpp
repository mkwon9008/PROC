#include "ChatServer.hpp"

/*
#�⺻ ��������.

(                     HEADER                   )(  CONTENTS )
| PacketCode | CheckSum | MsgType | PayloadSize | *Payload* |
	1byte		1byte		2byte		2byte	   SizeByte

CheckSum : �� MsgType, Payload�� �� ����Ʈ ���ϱ� % 256 (PayloadSize �� ������) (����ڿ� ���� ������ ��.������ �������� ���)
Payload�� ��ȣȭ�� �����߳�, ��ȣȭ�� �����߳ĸ� CheckSum ������ �Ǵ��ؾ� �Ѵ�.

����Ʈ(BYTE) : unsigned char

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

		//Ư�� Ű �� ��Ȳ�� ���� ����ó��.
		if (true)//üũ...
		{
			closesocket(g_ListenSocket);
			break;
		}
	}

	//���� ������ �����۾�.
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

	//1. listen() ���� ����.
	//Declaration ServSock
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	
	//�������� �Ҵ� ����üũ.
	if (hServSock == INVALID_SOCKET)
		ErrHandling("socket() Error");
	printf("socket() OK.\n");

	//2. ����ü ����.
	//ZeroMemory�� servAddr�а�, ���� �Ҵ�.
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(dfNETWORK_PORT);
	printf("SETservAddr OK.\n");


	//3. listen���Ͽ� ����ü �����ؼ� bind();
	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrHandling("bind() Err.");
	printf("bind() OK.\n");


	//4. binding �� �Ϸ� �� hServSock ���� listen() ����.
	//listen(���ϵ�ũ����, ��� �޽��� ť�� ����);
	if (listen(hServSock, 10) == SOCKET_ERROR)
		ErrHandling("listen() Err.");
	printf("listen() OK\n");

	//readset �ʱ�ȭ.
	FD_ZERO(&reads);
	//���ϵ�ũ���Ϳ� readset �߰�.
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