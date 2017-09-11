#pragma once
#ifndef __STREAMING_BUFFER_H__
#define __STREAMING_BUFFER_H__

#include <iostream>
#include <cstdlib>

#define BASE_BUFFER_SIZE 500

class CStreamQueue
{
private :
	char* _pBuffer;
	int _iFront, _iRear;
	int _iBufferSize;
	int _iBufferUsedSize;
public :

	CStreamQueue(void);
	CStreamQueue(int iSize);
	~CStreamQueue();

	//초기화.
	void Initial(int iBufferSize = BASE_BUFFER_SIZE);


	//전체 버퍼 사이즈.
	int	GetBufferSize(void);


	// 현재 사용중인 용량.
	int	GetUseSize(void);


	// 현재 버퍼에 남은 용량. (실제 사용할 수 있는 용량)
	int	GetFreeSize(void);


	//읽고 쓰는과정에서 버퍼의 끝에 도달하기 까지 얻을 수 있는 용량.(끊기지 않은 길이)
	int	GetSafeReadSize(void);	//int GetNotBrokenGetSize(void);
	int	GetSafeWriteSize(void); //int GetNotBrokenPutSize(void);


	//Rear를 통해 데이터 삽입.
	//return value : 넣는데 성공한 사이즈.
	int	Enqueue(char* chpData, int iSize);


	//Front 에서 데이타 가져옴. Front 이동.
	//return value : 빼는데 성공한 사이즈.
	int	Dequeue(char* chpDest, int iSize);


	//Front 에서 데이타 읽어옴. Front 고정.
	//return value : 읽어오는데 성공한 크기.
	int	Peek(char* chpDest, int iSize);


	//원하는 길이만큼 Front 위치 에서 Size만큼 삭제.
	void RemoveData(int iSize);

	//원하는 길이만큼 Front 위치 에서 Size만큼 이동. (음수값 올 수 없음)
	//return value : 이동한 Size 반환.
	int	MoveWritePos(int iSize);

	// 버퍼의 모든 데이터 삭제.
	void ClearBuffer(void);

	// 버퍼의 포인터 얻음.
	char* GetBufferPtr(void);

	// int Front 가 찌르고 있는 버퍼의 포인터를 얻음.
	char* GetPtrFrontToBuffer(void);	//char* GetReadBufferPtr(void);
	 
	//int Rear 가 찌르고 있는 버퍼의 포인터를 얻음.
	char* GetPtrRearToBuffer(void);		//char* GetWriteBufferPtr(void);

	bool isEmpty(void);

	bool isFull(void);
}; //endClass
#endif








