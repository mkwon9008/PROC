
//Auther:Kwon. Serialization Buffer. 170907.
#pragma once
#ifndef __SERIALIZATION_BUFFER_H__
#define __SERIALIZATION_BUFFER_H__

#include <iostream>
#include <cstdlib>
#include <windows.h>

class CPacket
{
private:
	char* m_buf;		//버퍼.
	int m_iBufWritePos;		//현재 사용중인 버퍼의 사이즈이면서 동시에 WritePos역할도 겸함.
	int m_bufSize;		//버퍼의 사이즈. (클래스 선언시 초기설정값)
	int m_iBufReadPos;	//버퍼의 ReadPos.

	enum en_ERRNO
	{
		PutDataParameterNull = -1,
		PutDataBufferSizeOver = -2,
		GetDataParameterNull = -3,
		GetDataMemcopyBufferToTemp = -4,
		GetDataMemcopyTempToDest = -5,
		PutDataMemcopySrcToBuffer = -6,
		GetDataRequestSizeOver = -7
	};

public:
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1024,		// 패킷의 기본 버퍼 사이즈.
		ePACKET_MAX_SIZE = 1024,	// 패킷의 최대 사이즈
	};

	CPacket();
	CPacket(int iBufferSize);
	//CPacket(const CPacket &clSrcPacket);

	virtual	~CPacket();


protected:
	// 패킷 초기화.
	void	Initial(int iBufferSize = eBUFFER_DEFAULT);
	void	Release(void);
	int		ErrCatch(errno_t err);

public:
	// 패킷 청소.
	void	Clear(void);

	// 버퍼 사이즈 얻기.
	int		GetBufferSize(void);

	// 패킷 실제 총 사이즈 (패킷 헤더부분을 제외한 실제 사용가능한 사이즈)
	int		GetEmptyPacketSize(void);

	// 이미 추출했던 패킷은 제외하고, 추출 가능한 패킷 사이즈.  ( 버퍼에있는 패킷사이즈 - 이미 추출한 패킷사이즈)
	int		GetRemainPacketSize(void);

	// 버퍼 포인터 얻기.
	char*	GetBufferReadPtr(void);

	// 버퍼의 m_buf[0] 의 ptr얻기.
	char* GetBufferZeroPtr(void);

	// 버퍼 Pos 이동. (음수이동은 안됨)
	// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	//버퍼에서 데이터 빼기. (char *)Dest 포인터. (int)Size.
	int		GetData(char *chpDest, int iSize);

	//버퍼에 데이터 삽입. (char *)Src 포인터. (int)SrcSize.
	int		PutData(char *chpSrc, int iSrcSize);


	//연산자 오버로딩.
	CPacket& operator << (char* cPtrData);
	CPacket& operator << (int& iData);
	CPacket& operator << (INT64& i64Data);
	CPacket& operator << (double& dData);
	CPacket& operator << (BYTE& byData);
	CPacket& operator << (WORD& wdData);

	//int operator >> (char* chpDest); 문자열은 뺄 값을 operator로 지정할 수 없음.. 그냥 packet.GetData() 로 쓰자.
	int operator >> (int& iData);
	int operator >> (INT64& i64Data);
	int operator >> (double& dData);
	int operator >> (BYTE& byData);
	int operator >> (WORD& wdData);
};

#endif
