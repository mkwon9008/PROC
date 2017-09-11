#include "StreamingBuffer.hpp"

CStreamQueue::CStreamQueue(void)
{
	Initial();
}

CStreamQueue::CStreamQueue(int iSize)
{
	Initial(iSize);
}

CStreamQueue::~CStreamQueue()
{
	delete[] _pBuffer;
}


//�ʱ�ȭ.
void CStreamQueue::Initial(int iBufferSize)
{
	if (iBufferSize <= 0 || iBufferSize == 1)
		fprintf(stderr, "CStreamQueue::Initial() err.\n");


	//������ �� [iBufferSize + 1]�� �����Ѵ�.
	//�� ������ [iBufferSize + 1]�� '����ť�� �ѹ���'�� �����ϱ� ���� �����̸�, ������� �ʴ� �����̴�.
	_pBuffer = new char[iBufferSize + 1];
	memset(_pBuffer, 0, (sizeof(char)* (iBufferSize + 1) ) );

	_iFront = 0;
	_iRear = 0;
	_iBufferSize = iBufferSize;
	_iBufferUsedSize = 0;
}


//��ü ���� ������.
int	CStreamQueue::GetBufferSize(void)
{
	return _iBufferSize;
}

// ���� ������� �뷮.
int	CStreamQueue::GetUseSize(void)
{
	return _iBufferUsedSize;
}

// ���� ���ۿ� ���� �뷮. (���� ����� �� �ִ� �뷮)
int	CStreamQueue::GetFreeSize(void)
{
	return (_iBufferSize) - (_iBufferUsedSize);
}


//�а� ���°������� ������ ���� �����ϱ� ���� ���� �� �ִ� �뷮.(������ ���� ����)
int	CStreamQueue::GetSafeReadSize(void)	//int GetNotBrokenGetSize(void);
{
	int tmp_iOldFront = _iFront; //_iFront���� �����Ҳ��� �̸� ����.
	int iCnt = 0;
	while (iCnt <= _iBufferSize)
	{
		if (_iFront >= _iBufferSize)
		{
			_iFront = tmp_iOldFront; //�� ��ȯ �� _iFront�� ������� �ǵ���.
			return iCnt;
		}

		_iFront++;
		iCnt++; 
	}
}

//�а� ���°������� ������ ���� �����ϱ� ���� ���� �� �ִ� �뷮.(������ ���� ����)
int	CStreamQueue::GetSafeWriteSize(void) //int GetNotBrokenPutSize(void);
{
	int tmp_iOldRear = _iRear;
	int iCnt = 0;

	while (iCnt <= _iBufferSize)
	{
		if (_iRear >= _iBufferSize )
		{
			_iRear = tmp_iOldRear;
			return iCnt;
		}

		_iRear++;
		iCnt++;
	}
}

//Rear�� ���� ������ ����.
//return value : �ִµ� ������ ������.
int	CStreamQueue::Enqueue(char* chpData, int iSize)
{
	//NULL pointer checkSum.
	if (chpData == NULL) 
		return -1;

	//Func start.
	int iCnt;
	int iFlag = 0;
	int iSafeWriteSize = GetSafeWriteSize();
	int iFreeSize = GetFreeSize();

	bool bIsFreeSize = false;
	bool bIsSafeWrite = false;

	//checkSum
	if (iSize > iFreeSize) //�������� ����� ���� ������ ���� ũ�� true;
		bIsFreeSize = true;

	if (iSize > iSafeWriteSize)  //�������� ����� SafeWrite������� ũ�� true;
		bIsSafeWrite = true;


	//Flag �� ����.
	//iSize�� ������������ �۰�, SafeWrite������ ���� �������.
	if (bIsFreeSize == false && bIsSafeWrite == false)
		iFlag = 1;

	//iSize�� ������������ �۰�, SafeWrite������ ���� Ŭ ���.
	if (bIsFreeSize == false && bIsSafeWrite == true)
		iFlag = 2;

	//iSize�� ������������ ũ��, SafeWrite������ ���� ���� ���.
	if (bIsFreeSize == true && bIsSafeWrite == false)
		iFlag = 3;

	//iSize�� �������� ���� ũ��, SafeWrite������ ���� Ŭ ���.
	if (bIsFreeSize == true && bIsSafeWrite == true)
		iFlag = 4;

	/*
	1. �Ѵ� ó�� �����ְ� �׳� �ֱ⸸ �ص� ��.
	2. SafeWrite����� ó�� ���ָ� ��.
	3. �ϴ� �Ǵµ������� �ְ� ���� �����ŭ ��ȯ.
	4. ��ü������.
	*/

	switch (iFlag)
	{
	case 1:
		{
			iCnt = 0;
			while (iCnt < iSize)
			{
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize �� ���� ������ŭ �߰�.
			return iCnt; 
		}

	case 2:
		{
			iCnt = 0;
			while (iCnt < iSafeWriteSize) //iSafeWriteSize������ŭ�� �ϴ� ����. +1�� _pBuffer[iBufferSize+1�ؼ�]
			{								//while Loop Ƚ���� iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}//����µ����� ���� �Ϸ�.

			_iRear = 0;
			while (iCnt < iSize)//_pBuffer[0]���� �����ؼ� �Ʊ� ���䵥������ ���� �� ��ŭ �ٽ� ����.
			{					//while Loop Ƚ���� iSize - iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize �� ���� ������ŭ �߰�.
			return iCnt;
		}

	case 3:
		{
			iCnt = 0;
			while (iCnt < iFreeSize)	//���������ŭ �Ǵµ����� ����.
			{
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize �� ���� ������ŭ �߰�.
			return iCnt;
		}

	case 4:
		{
			iCnt = 0;
			while (iCnt < iSafeWriteSize) //iSafeWriteSize������ŭ�� �ϴ� ����.
			{								//while Loop Ƚ���� iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}//����µ����� ���� �Ϸ�.

			_iRear = 0;
			while (iCnt < iFreeSize)//_pBuffer[0]���� �����ؼ� �Ʊ� ���䵥������ ���� �� ��ŭ �ٽ� ����.
			{					//while Loop Ƚ���� iFreeSize - iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize �� ���� ������ŭ �߰�.
			return iCnt;
		}

	default:
		{
			return -2;
		}
	}

}



//Front ���� ����Ÿ ������. Front �̵�.
//return value : ���µ� ������ ������.
int	CStreamQueue::Dequeue(char* chpDest, int iSize)
{
	//NULL pointer checkSum.
	if (chpDest == NULL)
		return -1;

	//isEmpty() checkSum.
	//if (isEmpty() == true) //ť�� ��������� -2 ����.
	//	return -2;

	//func start.
	int iUseSize = GetUseSize();
	int iSafeReadSize = GetSafeReadSize();
	int iCnt;

	if (iSize > iUseSize)	//�������� ����� ���� ������� ������ ���� ũ��.
		iSize = iUseSize;	//������� �����ŭ�� �̰ڴ�.
	
	if (iSize > iSafeReadSize) //�������� ����� iSafeReadSize���� ũ��
	{
		iCnt = 0;
		while (iCnt < iSafeReadSize ) //iSafeWriteSize������ŭ�� �ϴ� ����.
		{								//while Loop Ƚ���� iSafeReadSize;
			_iFront++;
			chpDest[iCnt] = _pBuffer[_iFront];
			memset(&_pBuffer[_iFront], 0, sizeof(_pBuffer[_iFront]));
			iCnt++;
		}//����µ����� �̱� �Ϸ�.

		_iFront = 0;
		while (iCnt < iSize)//_pBuffer[0]���� �����ؼ� �Ʊ� ���䵥������ ���� �� ��ŭ �ٽ� ����.
		{					//while Loop Ƚ���� iSize - iSafeReadSize;
			_iFront++;
			chpDest[iCnt] = _pBuffer[_iFront];
			memset(&_pBuffer[_iFront], 0, sizeof(_pBuffer[_iFront]));
			iCnt++;
		}
		_iBufferUsedSize = _iBufferUsedSize - iCnt;//_iBufferUsedSize �� iCnt��ŭ ��.
		return iCnt; //�� ������ ��ȯ
	}
	else //�������� ����� iSafeReadSize���� ������
	{
		iCnt = 0;
		while (iCnt < iSize)//�������� �����ŭ ����.
		{					//while Loop Ƚ���� iSize;
			_iFront++;
			chpDest[iCnt] = _pBuffer[_iFront];
			memset(&_pBuffer[_iFront], 0, sizeof(_pBuffer[_iFront]));
			iCnt++;
		}
		_iBufferUsedSize = _iBufferUsedSize - iCnt;//_iBufferUsedSize �� iCnt��ŭ ��.
		return iCnt; //�� ������ ��ȯ
	}

}

//Front ���� ����Ÿ �о��. Front ����.
//return value : �о���µ� ������ ũ��.
int	CStreamQueue::Peek(char* chpDest, int iSize)
{
	//NULL pointer checkSum.
	if (chpDest == NULL)
		return -1;

	//isEmpty() checkSum.
	if (isEmpty() == true) //ť�� ��������� -2 ����.
		return -2;

	//func start.
	int iUseSize = GetUseSize();
	int tmp_iOldFront = _iFront;
	int iCnt;
	int iSafeReadSize = GetSafeReadSize();

	if (iSize > iUseSize)	//�������� ����� ���� ������� ������ ���� ũ��.
		iSize = iUseSize;	//������� �����ŭ�� �̰ڴ�.

	if (iSize > iSafeReadSize) //peek�Ϸ��� ����� �߰��� �����.
	{
		iCnt = 0;
		while (iCnt < iSafeReadSize) //iSafeWriteSize������ŭ�� �ϴ� ����.
		{								//while Loop Ƚ���� iSafeReadSize;
			chpDest[iCnt] = _pBuffer[_iFront];
			_iFront++;
			iCnt++;
		}//����µ����� �̱� �Ϸ�.

		_iFront = 0;
		while (iCnt < iSize)//_pBuffer[0]���� �����ؼ� �Ʊ� ���䵥������ ���� �� ��ŭ �ٽ� ����.
		{					//while Loop Ƚ���� iSize - iSafeReadSize;
			chpDest[iCnt] = _pBuffer[_iFront];
			_iFront++;
			iCnt++;
		}
		_iFront = tmp_iOldFront;
		return iCnt + 1; //�� ������ ��ȯ�̴ϱ� +1 ����.
	}

	else //�Ȳ����.
	{
		iCnt = 0;
		while (iCnt < iSize)//_pBuffer[0]���� �����ؼ� �Ʊ� ���䵥������ ���� �� ��ŭ �ٽ� ����.
		{					//while Loop Ƚ���� iSize - iSafeReadSize;
			chpDest[iCnt] = _pBuffer[_iFront];
			_iFront++;
			iCnt++;
		}
		_iFront = tmp_iOldFront;
		return iCnt + 1; //�� ������ ��ȯ�̴ϱ� +1 ����.
	}

}


//���ϴ� ���̸�ŭ Front ��ġ ���� Size��ŭ ����.
void CStreamQueue::RemoveData(int iSize)
{
	int tmp_iOldFront = _iFront;
	int iCnt = 0;
	while (iCnt < iSize)
	{
		if (_iFront == _iBufferSize)
		{
			_iFront = 0; 
		}

		memset(&_pBuffer[_iFront], 0, sizeof(char));
		_iFront++;
		iCnt++;
	}

	_iFront = tmp_iOldFront;
}


//���ϴ� ���̸�ŭ Front ��ġ ���� Size��ŭ �̵�. (������ �� �� ����)
//return value : �̵��� Size ��ȯ.
int	CStreamQueue::MoveWritePos(int iSize)
{
	if (iSize <= 0)
		return -1;

	int iCnt = 0;
	while (iCnt < iSize)
	{
		if (_iFront == _iBufferSize) //_iFront�� _iBufferSize-1�� �Ǿ��ٸ�
		{
			_iFront = 0; //����ť�� �����ϱ� ���� 0��° �迭�� ���ư�.
		}
		_iFront++;
		iCnt++;
	}
	return iCnt;
}



// ������ ��� ������ ����.
void CStreamQueue::ClearBuffer(void)
{
	memset(_pBuffer, 0, ( sizeof(char) * (_iBufferSize) ) );
	_iFront = 0;
	_iRear = 0;
	_iBufferUsedSize = 0;
}

// ������ ������ ����.
char* CStreamQueue::GetBufferPtr(void)
{
	return _pBuffer;
}

// int Front �� ��� �ִ� ������ �����͸� ����.
char* CStreamQueue::GetPtrFrontToBuffer(void)	//char* GetReadBufferPtr(void);
{
	return &_pBuffer[_iFront];
}

//int Rear �� ��� �ִ� ������ �����͸� ����.
char* CStreamQueue::GetPtrRearToBuffer(void)	//char* GetWriteBufferPtr(void);
{
	return &_pBuffer[_iRear];
}

bool CStreamQueue::isEmpty(void)
{
	if (_iFront == _iRear)
		return true;
	
	return false;
}

bool CStreamQueue::isFull(void)
{
	int tmp_iOldRear = _iRear;

	if (_iRear == _iBufferSize)
		_iRear = -1;

	if ((_iRear + 1) == _iFront)
	{
		_iRear = tmp_iOldRear;
		return true;
	}
	_iRear = tmp_iOldRear;
	return false;
}