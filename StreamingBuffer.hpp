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

	//�ʱ�ȭ.
	void Initial(int iBufferSize = BASE_BUFFER_SIZE);


	//��ü ���� ������.
	int	GetBufferSize(void);


	// ���� ������� �뷮.
	int	GetUseSize(void);


	// ���� ���ۿ� ���� �뷮. (���� ����� �� �ִ� �뷮)
	int	GetFreeSize(void);


	//�а� ���°������� ������ ���� �����ϱ� ���� ���� �� �ִ� �뷮.(������ ���� ����)
	int	GetSafeReadSize(void);	//int GetNotBrokenGetSize(void);
	int	GetSafeWriteSize(void); //int GetNotBrokenPutSize(void);


	//Rear�� ���� ������ ����.
	//return value : �ִµ� ������ ������.
	int	Enqueue(char* chpData, int iSize);


	//Front ���� ����Ÿ ������. Front �̵�.
	//return value : ���µ� ������ ������.
	int	Dequeue(char* chpDest, int iSize);


	//Front ���� ����Ÿ �о��. Front ����.
	//return value : �о���µ� ������ ũ��.
	int	Peek(char* chpDest, int iSize);


	//���ϴ� ���̸�ŭ Front ��ġ ���� Size��ŭ ����.
	void RemoveData(int iSize);

	//���ϴ� ���̸�ŭ Front ��ġ ���� Size��ŭ �̵�. (������ �� �� ����)
	//return value : �̵��� Size ��ȯ.
	int	MoveWritePos(int iSize);

	// ������ ��� ������ ����.
	void ClearBuffer(void);

	// ������ ������ ����.
	char* GetBufferPtr(void);

	// int Front �� ��� �ִ� ������ �����͸� ����.
	char* GetPtrFrontToBuffer(void);	//char* GetReadBufferPtr(void);
	 
	//int Rear �� ��� �ִ� ������ �����͸� ����.
	char* GetPtrRearToBuffer(void);		//char* GetWriteBufferPtr(void);

	bool isEmpty(void);

	bool isFull(void);
}; //endClass
#endif








