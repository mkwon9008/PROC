#pragma once

#ifndef __CHAT_SERVER_H__
#define __CHAT_SERVER_H__

#pragma warning(disable : 4996)
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <cstdlib>
#include <WinSock2.h>
#include <vector>
#include <map>
#include <list>
#include "Protocol.hpp"
#include "SerializationBuffer\SerializationBuffer.hpp"
#include "StreamingBuffer\StreamingBuffer.hpp"

#define BUFSIZE 256
#define dfRECV_BUFF 512

typedef struct st_CLIENT
{
	SOCKET			Socket;
	SOCKADDR_IN		ClientAddr;
	
	CStreamQueue SendQueue;
	CStreamQueue RecvQueue;
	
	DWORD dwUserNO;
	DWORD dwEnterRoomNO;

	WCHAR szNickname[dfNICK_MAX_LEN];
	bool bIsRobby;

}CLIENT;

typedef struct st_CHAT_ROOM
{
	DWORD	dwRoomNO;
	WCHAR	szTitle[dfCHAT_ROOM_TITLE_MAX_LEN];

	std::list<DWORD> UserList;
}CHATROOM;

extern std::map<DWORD, st_CLIENT*>		g_ClientMap;	//클라 관리용 맵.
extern std::map<DWORD, st_CHAT_ROOM*>	g_RoomMap;		//방 관리용 맵.

extern SOCKET g_ListenSocket;

/* 실제 서비스 시 이렇게 사용하면 절대 안됨. */
//유저 고유키, 방 고유 키를 만들 전역 변수, 할당 시 마다 +1 해서 사용.
extern DWORD g_dwKey_UserNO;
extern DWORD g_dwKey_RoomNO;
/*********************************************/

bool NetworkInit(void);
void NetworkProcess(void);
void SelectSocket(DWORD* dwpTableNO, SOCKET* pTableSocket, FD_SET* pReadSet, FD_SET* pWriteSet);
void ErrHandling(char *message);
void netProc_Accept(void);


#endif

