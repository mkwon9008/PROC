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


//초기화.
void CStreamQueue::Initial(int iBufferSize)
{
	if (iBufferSize <= 0 || iBufferSize == 1)
		fprintf(stderr, "CStreamQueue::Initial() err.\n");


	//선언할 때 [iBufferSize + 1]로 선언한다.
	//맨 마지막 [iBufferSize + 1]는 '원형큐의 한바퀴'를 구분하기 위한 공간이며, 사용하지 않는 공간이다.
	_pBuffer = new char[iBufferSize + 1];
	memset(_pBuffer, 0, (sizeof(char)* (iBufferSize + 1) ) );

	_iFront = 0;
	_iRear = 0;
	_iBufferSize = iBufferSize;
	_iBufferUsedSize = 0;
}


//전체 버퍼 사이즈.
int	CStreamQueue::GetBufferSize(void)
{
	return _iBufferSize;
}

// 현재 사용중인 용량.
int	CStreamQueue::GetUseSize(void)
{
	return _iBufferUsedSize;
}

// 현재 버퍼에 남은 용량. (실제 사용할 수 있는 용량)
int	CStreamQueue::GetFreeSize(void)
{
	return (_iBufferSize) - (_iBufferUsedSize);
}


//읽고 쓰는과정에서 버퍼의 끝에 도달하기 까지 얻을 수 있는 용량.(끊기지 않은 길이)
int	CStreamQueue::GetSafeReadSize(void)	//int GetNotBrokenGetSize(void);
{
	int tmp_iOldFront = _iFront; //_iFront값을 조작할꺼라서 미리 저장.
	int iCnt = 0;
	while (iCnt <= _iBufferSize)
	{
		if (_iFront >= _iBufferSize)
		{
			_iFront = tmp_iOldFront; //값 반환 전 _iFront를 원래대로 되돌림.
			return iCnt;
		}

		_iFront++;
		iCnt++; 
	}
}

//읽고 쓰는과정에서 버퍼의 끝에 도달하기 까지 얻을 수 있는 용량.(끊기지 않은 길이)
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

//Rear를 통해 데이터 삽입.
//return value : 넣는데 성공한 사이즈.
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
	if (iSize > iFreeSize) //넣으려는 사이즈가 프리 사이즈 보다 크면 true;
		bIsFreeSize = true;

	if (iSize > iSafeWriteSize)  //넣으려는 사이즈가 SafeWrite사이즈보다 크면 true;
		bIsSafeWrite = true;


	//Flag 값 셋팅.
	//iSize가 여유공간보다 작고, SafeWrite사이즈 보다 작을경우.
	if (bIsFreeSize == false && bIsSafeWrite == false)
		iFlag = 1;

	//iSize가 여유공간보다 작고, SafeWrite사이즈 보다 클 경우.
	if (bIsFreeSize == false && bIsSafeWrite == true)
		iFlag = 2;

	//iSize가 여유공간보다 크고, SafeWrite사이즈 보다 작을 경우.
	if (bIsFreeSize == true && bIsSafeWrite == false)
		iFlag = 3;

	//iSize가 여유공간 보다 크고, SafeWrite사이즈 보다 클 경우.
	if (bIsFreeSize == true && bIsSafeWrite == true)
		iFlag = 4;

	/*
	1. 둘다 처리 안해주고 그냥 넣기만 해도 됨.
	2. SafeWrite사이즈만 처리 해주면 됨.
	3. 일단 되는데까지만 넣고 넣은 사이즈만큼 반환.
	4. 총체적난국.
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
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize 에 넣은 갯수만큼 추가.
			return iCnt; 
		}

	case 2:
		{
			iCnt = 0;
			while (iCnt < iSafeWriteSize) //iSafeWriteSize갯수만큼만 일단 넣음. +1은 _pBuffer[iBufferSize+1해서]
			{								//while Loop 횟수는 iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}//끊기는데까지 삽입 완료.

			_iRear = 0;
			while (iCnt < iSize)//_pBuffer[0]부터 시작해서 아까 끊긴데서부터 남는 수 만큼 다시 넣음.
			{					//while Loop 횟수는 iSize - iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize 에 넣은 갯수만큼 추가.
			return iCnt;
		}

	case 3:
		{
			iCnt = 0;
			while (iCnt < iFreeSize)	//프리사이즈만큼 되는데까지 넣음.
			{
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize 에 넣은 갯수만큼 추가.
			return iCnt;
		}

	case 4:
		{
			iCnt = 0;
			while (iCnt < iSafeWriteSize) //iSafeWriteSize갯수만큼만 일단 넣음.
			{								//while Loop 횟수는 iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}//끊기는데까지 삽입 완료.

			_iRear = 0;
			while (iCnt < iFreeSize)//_pBuffer[0]부터 시작해서 아까 끊긴데서부터 남는 수 만큼 다시 넣음.
			{					//while Loop 횟수는 iFreeSize - iSafeWriteSize;
				_iRear++;
				_pBuffer[_iRear] = chpData[iCnt];
				iCnt++;
			}
			_iBufferUsedSize = _iBufferUsedSize + iCnt; //_iBufferUsedSize 에 넣은 갯수만큼 추가.
			return iCnt;
		}

	default:
		{
			return -2;
		}
	}

}



//Front 에서 데이타 가져옴. Front 이동.
//return value : 빼는데 성공한 사이즈.
int	CStreamQueue::Dequeue(char* chpDest, int iSize)
{
	//NULL pointer checkSum.
	if (chpDest == NULL)
		return -1;

	//isEmpty() checkSum.
	//if (isEmpty() == true) //큐가 비어있으면 -2 리턴.
	//	return -2;

	//func start.
	int iUseSize = GetUseSize();
	int iSafeReadSize = GetSafeReadSize();
	int iCnt;

	if (iSize > iUseSize)	//뽑으려는 사이즈가 현재 사용중인 사이즈 보다 크면.
		iSize = iUseSize;	//사용중인 사이즈만큼만 뽑겠다.
	
	if (iSize > iSafeReadSize) //뽑으려는 사이즈가 iSafeReadSize보다 크면
	{
		iCnt = 0;
		while (iCnt < iSafeReadSize ) //iSafeWriteSize갯수만큼만 일단 뽑음.
		{								//while Loop 횟수는 iSafeReadSize;
			_iFront++;
			chpDest[iCnt] = _pBuffer[_iFront];
			memset(&_pBuffer[_iFront], 0, sizeof(_pBuffer[_iFront]));
			iCnt++;
		}//끊기는데까지 뽑기 완료.

		_iFront = 0;
		while (iCnt < iSize)//_pBuffer[0]부터 시작해서 아까 끊긴데서부터 남는 수 만큼 다시 뽑음.
		{					//while Loop 횟수는 iSize - iSafeReadSize;
			_iFront++;
			chpDest[iCnt] = _pBuffer[_iFront];
			memset(&_pBuffer[_iFront], 0, sizeof(_pBuffer[_iFront]));
			iCnt++;
		}
		_iBufferUsedSize = _iBufferUsedSize - iCnt;//_iBufferUsedSize 에 iCnt만큼 뺌.
		return iCnt; //뺀 갯수를 반환
	}
	else //뽑으려는 사이즈가 iSafeReadSize보다 작으면
	{
		iCnt = 0;
		while (iCnt < iSize)//뽑으려는 사이즈만큼 뽑음.
		{					//while Loop 횟수는 iSize;
			_iFront++;
			chpDest[iCnt] = _pBuffer[_iFront];
			memset(&_pBuffer[_iFront], 0, sizeof(_pBuffer[_iFront]));
			iCnt++;
		}
		_iBufferUsedSize = _iBufferUsedSize - iCnt;//_iBufferUsedSize 에 iCnt만큼 뺌.
		return iCnt; //뺀 갯수를 반환
	}

}

//Front 에서 데이타 읽어옴. Front 고정.
//return value : 읽어오는데 성공한 크기.
int	CStreamQueue::Peek(char* chpDest, int iSize)
{
	//NULL pointer checkSum.
	if (chpDest == NULL)
		return -1;

	//isEmpty() checkSum.
	if (isEmpty() == true) //큐가 비어있으면 -2 리턴.
		return -2;

	//func start.
	int iUseSize = GetUseSize();
	int tmp_iOldFront = _iFront;
	int iCnt;
	int iSafeReadSize = GetSafeReadSize();

	if (iSize > iUseSize)	//뽑으려는 사이즈가 현재 사용중인 사이즈 보다 크면.
		iSize = iUseSize;	//사용중인 사이즈만큼만 뽑겠다.

	if (iSize > iSafeReadSize) //peek하려는 사이즈가 중간에 끊기면.
	{
		iCnt = 0;
		while (iCnt < iSafeReadSize) //iSafeWriteSize갯수만큼만 일단 뽑음.
		{								//while Loop 횟수는 iSafeReadSize;
			chpDest[iCnt] = _pBuffer[_iFront];
			_iFront++;
			iCnt++;
		}//끊기는데까지 뽑기 완료.

		_iFront = 0;
		while (iCnt < iSize)//_pBuffer[0]부터 시작해서 아까 끊긴데서부터 남는 수 만큼 다시 뽑음.
		{					//while Loop 횟수는 iSize - iSafeReadSize;
			chpDest[iCnt] = _pBuffer[_iFront];
			_iFront++;
			iCnt++;
		}
		_iFront = tmp_iOldFront;
		return iCnt + 1; //뺀 갯수를 반환이니까 +1 해줌.
	}

	else //안끊기면.
	{
		iCnt = 0;
		while (iCnt < iSize)//_pBuffer[0]부터 시작해서 아까 끊긴데서부터 남는 수 만큼 다시 뽑음.
		{					//while Loop 횟수는 iSize - iSafeReadSize;
			chpDest[iCnt] = _pBuffer[_iFront];
			_iFront++;
			iCnt++;
		}
		_iFront = tmp_iOldFront;
		return iCnt + 1; //뺀 갯수를 반환이니까 +1 해줌.
	}

}


//원하는 길이만큼 Front 위치 에서 Size만큼 삭제.
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


//원하는 길이만큼 Front 위치 에서 Size만큼 이동. (음수값 올 수 없음)
//return value : 이동한 Size 반환.
int	CStreamQueue::MoveWritePos(int iSize)
{
	if (iSize <= 0)
		return -1;

	int iCnt = 0;
	while (iCnt < iSize)
	{
		if (_iFront == _iBufferSize) //_iFront가 _iBufferSize-1이 되었다면
		{
			_iFront = 0; //원형큐를 유지하기 위해 0번째 배열로 돌아감.
		}
		_iFront++;
		iCnt++;
	}
	return iCnt;
}



// 버퍼의 모든 데이터 삭제.
void CStreamQueue::ClearBuffer(void)
{
	memset(_pBuffer, 0, ( sizeof(char) * (_iBufferSize) ) );
	_iFront = 0;
	_iRear = 0;
	_iBufferUsedSize = 0;
}

// 버퍼의 포인터 얻음.
char* CStreamQueue::GetBufferPtr(void)
{
	return _pBuffer;
}

// int Front 가 찌르고 있는 버퍼의 포인터를 얻음.
char* CStreamQueue::GetPtrFrontToBuffer(void)	//char* GetReadBufferPtr(void);
{
	return &_pBuffer[_iFront];
}

//int Rear 가 찌르고 있는 버퍼의 포인터를 얻음.
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