
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
	char* m_buf;		//����.
	int m_iBufWritePos;		//���� ������� ������ �������̸鼭 ���ÿ� WritePos���ҵ� ����.
	int m_bufSize;		//������ ������. (Ŭ���� ����� �ʱ⼳����)
	int m_iBufReadPos;	//������ ReadPos.

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
		eBUFFER_DEFAULT = 1024,		// ��Ŷ�� �⺻ ���� ������.
		ePACKET_MAX_SIZE = 1024,	// ��Ŷ�� �ִ� ������
	};

	CPacket();
	CPacket(int iBufferSize);
	//CPacket(const CPacket &clSrcPacket);

	virtual	~CPacket();


protected:
	// ��Ŷ �ʱ�ȭ.
	void	Initial(int iBufferSize = eBUFFER_DEFAULT);
	void	Release(void);
	int		ErrCatch(errno_t err);

public:
	// ��Ŷ û��.
	void	Clear(void);

	// ���� ������ ���.
	int		GetBufferSize(void);

	// ��Ŷ ���� �� ������ (��Ŷ ����κ��� ������ ���� ��밡���� ������)
	int		GetEmptyPacketSize(void);

	// �̹� �����ߴ� ��Ŷ�� �����ϰ�, ���� ������ ��Ŷ ������.  ( ���ۿ��ִ� ��Ŷ������ - �̹� ������ ��Ŷ������)
	int		GetRemainPacketSize(void);

	// ���� ������ ���.
	char*	GetBufferReadPtr(void);

	// ������ m_buf[0] �� ptr���.
	char* GetBufferZeroPtr(void);

	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	//���ۿ��� ������ ����. (char *)Dest ������. (int)Size.
	int		GetData(char *chpDest, int iSize);

	//���ۿ� ������ ����. (char *)Src ������. (int)SrcSize.
	int		PutData(char *chpSrc, int iSrcSize);


	//������ �����ε�.
	CPacket& operator << (char* cPtrData);
	CPacket& operator << (int& iData);
	CPacket& operator << (INT64& i64Data);
	CPacket& operator << (double& dData);
	CPacket& operator << (BYTE& byData);
	CPacket& operator << (WORD& wdData);

	//int operator >> (char* chpDest); ���ڿ��� �� ���� operator�� ������ �� ����.. �׳� packet.GetData() �� ����.
	int operator >> (int& iData);
	int operator >> (INT64& i64Data);
	int operator >> (double& dData);
	int operator >> (BYTE& byData);
	int operator >> (WORD& wdData);
};

#endif