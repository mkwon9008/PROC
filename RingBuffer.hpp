#pragma once
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__
#include <iostream>
enum enRINGBUFFER
{
	enDefaultBufferSize = 500
};

class CRingBuffer
{
private:
	char* _chpBuffer;		//����.
	char* _chpWritePtr;		//���� ������.
	char* _chpReadPtr;		//�б� ������.
	const char* _chpBufferEnd;	//������ �� ������.

	unsigned int _ui32BufferSize;	//���� ��Ż ������.
	unsigned int _ui32UseSize;		//������� ������.

	bool init(const int i32BufferEnd);

public:
	CRingBuffer(int i32BufferSize = enRINGBUFFER::enDefaultBufferSize);
	~CRingBuffer();

	/*GET***********************************************************/
	int		GetBufferSize(void);				// ������ �� ������ ���.
	int		GetUseSize(void);					// ���� ������� �뷮 ���.
	int		GetFreeSize(void);					// ���� ���ۿ� ���� �뷮 ���.
	char*	GetWriteBufferPtr(void);			// ������ Rear�κ�. ���� �κ��� ������.
	char*	GetReadBufferPtr(void);				// ������ Front�κ�. �д� �κ��� ������.
	int		GetNotBrokenWriteSize(void);		// ���� �����ͷ� �ܺο��� �ѹ濡 �� �� �ִ� ����.
	int		GetNotBrokenReadSize(void);			// ���� �����ͷ� �ܺο��� �ѹ濡 ���� �� �ִ� ����.
	char*	GetBufferBeginPtr(void);			// ������ ù ���ۺκ� ������.
	/***************************************************************/

	/*Funtion*******************************************************/
	int		Enqueue(char* chpData, int i32Size);	// WritePos �� ����Ÿ ����.
	int		Dequeue(char* chpDest, int i32Size);	// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
	int		Peek(char* chpDest, int i32Size);				// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	/***************************************************************/

	/*Utility*******************************************************/
	bool	IsEmpty(void);										// ���۰� ������� �˻�.
	bool	IsFull(void);										// ���۰� ���� á���� �˻�.
	int		MoveWritePos(int i32Size, bool bBeginFlag = false);	// WritePtr �̵�. (Enqueue�� �� ���).
	int		MoveReadPos(int i32Size, bool bBeginFlag = false);	// ReadPtr �̵�. (������ ������ ����).
	void	ClearBuffer(void);									// ������ ��� ������ ����. (������ ������ �ʿ����, WritePos, ReadPos�� �ʱⰪ���� �������ϸ��)
	/***************************************************************/
};
#endif