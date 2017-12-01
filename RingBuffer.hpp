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
	char* _chpBuffer;		//버퍼.
	char* _chpWritePtr;		//쓰기 포인터.
	char* _chpReadPtr;		//읽기 포인터.
	const char* _chpBufferEnd;	//버퍼의 끝 포인터.

	unsigned int _ui32BufferSize;	//버퍼 토탈 사이즈.
	unsigned int _ui32UseSize;		//사용중인 사이즈.

	bool init(const int i32BufferEnd);

public:
	CRingBuffer(int i32BufferSize = enRINGBUFFER::enDefaultBufferSize);
	~CRingBuffer();

	/*GET***********************************************************/
	int		GetBufferSize(void);				// 버퍼의 총 사이즈 얻기.
	int		GetUseSize(void);					// 현재 사용중인 용량 얻기.
	int		GetFreeSize(void);					// 현재 버퍼에 남은 용량 얻기.
	char*	GetWriteBufferPtr(void);			// 버퍼의 Rear부분. 쓰는 부분의 포인터.
	char*	GetReadBufferPtr(void);				// 버퍼의 Front부분. 읽는 부분의 포인터.
	int		GetNotBrokenWriteSize(void);		// 버퍼 포인터로 외부에서 한방에 쓸 수 있는 길이.
	int		GetNotBrokenReadSize(void);			// 버퍼 포인터로 외부에서 한방에 읽을 수 있는 길이.
	char*	GetBufferBeginPtr(void);			// 버퍼의 첫 시작부분 포인터.
	/***************************************************************/

	/*Funtion*******************************************************/
	int		Enqueue(char* chpData, int i32Size);	// WritePos 에 데이타 넣음.
	int		Dequeue(char* chpDest, int i32Size);	// ReadPos 에서 데이타 가져옴. ReadPos 이동.
	int		Peek(char* chpDest, int i32Size);				// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
	/***************************************************************/

	/*Utility*******************************************************/
	bool	IsEmpty(void);										// 버퍼가 비었는지 검사.
	bool	IsFull(void);										// 버퍼가 가득 찼는지 검사.
	int		MoveWritePos(int i32Size, bool bBeginFlag = false);	// WritePtr 이동. (Enqueue할 때 사용).
	int		MoveReadPos(int i32Size, bool bBeginFlag = false);	// ReadPtr 이동. (데이터 삭제와 동일).
	void	ClearBuffer(void);									// 버퍼의 모든 데이터 삭제. (실제로 삭제할 필요없이, WritePos, ReadPos만 초기값으로 리셋팅하면됨)
	/***************************************************************/
};
#endif