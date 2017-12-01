#include "RingBuffer.hpp"

CRingBuffer::CRingBuffer(int i32BufferSize)
	: _ui32BufferSize(i32BufferSize), _ui32UseSize(0),
	_chpBuffer(nullptr), _chpWritePtr(nullptr), _chpReadPtr(nullptr), _chpBufferEnd(nullptr)
{
	//클래스 생성자.
	if (init(i32BufferSize - 1) == true)
	{
		//RingBuffer initialize fail.
		return;
	}
}

CRingBuffer::~CRingBuffer()
{
	delete[] _chpBuffer;
	_chpBuffer = nullptr;
}

//초기화 함수. (Param : 버퍼사이즈의 -1 값을 넣어서 해당 버퍼의 끝을 가리키게 하는 const BufferEnd셋팅.)
bool CRingBuffer::init(const int i32BufferEnd)
{
	_chpBuffer		= new char[CRingBuffer::GetBufferSize()];
	_chpWritePtr	= _chpBuffer;
	_chpReadPtr		= _chpBuffer;
	_chpBufferEnd = &_chpBuffer[i32BufferEnd];
	if (_chpBuffer == nullptr || _chpWritePtr == nullptr || _chpReadPtr == nullptr || _chpBufferEnd == nullptr)
	{
		return true;
	}
	memset(_chpBuffer, 0, GetBufferSize());
	return false;
}

/*GET****************************************************************************************/
// 버퍼의 총 사이즈 얻기.
int	CRingBuffer::GetBufferSize(void)
{
	return _ui32BufferSize;
}

// 현재 사용중인 용량 얻기.
int	CRingBuffer::GetUseSize(void)
{
	return _ui32UseSize;
}

// 현재 버퍼에 남은 용량 얻기.
int	CRingBuffer::GetFreeSize(void)
{
	return (_ui32BufferSize) - (_ui32UseSize);
}

// 버퍼의 Rear부분. 쓰는 부분의 포인터.
char* CRingBuffer::GetWriteBufferPtr(void)
{
	return _chpWritePtr;
}

// 버퍼의 Front부분. 읽는 부분의 포인터.
char* CRingBuffer::GetReadBufferPtr(void)
{
	return _chpReadPtr;
}

// 버퍼 포인터로 외부에서 한방에 쓸 수 있는 길이.
int	CRingBuffer::GetNotBrokenWriteSize(void)
{
	char* chpTempWritePtr = GetWriteBufferPtr();
	int i32AbleWriteSize = 0;

	if (chpTempWritePtr == _chpBufferEnd)
	{
		return i32AbleWriteSize;
	}
	else
	{
		while (chpTempWritePtr != _chpBufferEnd)
		{
			chpTempWritePtr++;
			i32AbleWriteSize++;
		}
	}

	return i32AbleWriteSize;
}

// 버퍼 포인터로 외부에서 한방에 읽을 수 있는 길이.
int	CRingBuffer::GetNotBrokenReadSize(void)
{
	char* chpTempReadPtr = GetReadBufferPtr();
	int i32AbleReadSize = 0;

	if (chpTempReadPtr == _chpBufferEnd)
	{
		return i32AbleReadSize;
	}
	else
	{
		while (chpTempReadPtr != _chpBufferEnd)
		{
			chpTempReadPtr++;
			i32AbleReadSize++;
		}
	}

	return i32AbleReadSize;
}

// 버퍼의 첫 시작부분 포인터.
char* CRingBuffer::GetBufferBeginPtr(void)
{
	return _chpBuffer;
}
/********************************************************************************************/



/*Funtion************************************************************************************/
// WritePos 에 데이타 넣음.
//chpData:넣으려는 데이터, i32Size : 넣으려는 데이터의 수, return : 넣기성공한 수.
int CRingBuffer::Enqueue(char* chpData, int i32Size)
{
	int i32AbleWriteSize = GetNotBrokenWriteSize();
	int i32FreeSize = GetFreeSize();

	//1. 넣으려는 사이즈 > 사용가능한 사이즈. (가능한데까지만 넣는다.)
	if (i32Size > i32FreeSize)
	{
		i32Size = i32FreeSize;
	}
	
	//2. 끊기는 부분이 있다.
	if (i32Size > i32AbleWriteSize)
	{
		//RemainSize(끊기는데까지 넣고 남는용량) = Size(넣으려는용량) - AbleWriteSize(끊기는데까지 용량);
		int i32RemainSize = i32Size - i32AbleWriteSize;

		//memcpy를 두번 해야함. 일단 끊기는데까지 넣음. (WritePos부터 버퍼의 끊기는부분까지)
		memcpy_s(GetWriteBufferPtr(), i32AbleWriteSize, chpData, i32AbleWriteSize);
		
		//두번째 memcpy. 끊기고 남는 사이즈만큼 넣음. (버퍼의 첫부분 부터 남는 사이즈까지)
		memcpy_s(GetBufferBeginPtr(), i32RemainSize, chpData + i32AbleWriteSize, i32RemainSize);

		//WritePos이동. (버퍼의 첫부분에서 ReaminSize만큼)
		MoveWritePos(i32RemainSize, true);

		//사용중인 용량 계산.
		_ui32UseSize += i32Size;

		//넣은값 리턴.
		return i32Size;
	}
	
	//3. 끊기는 부분이 없다.
	memcpy_s(GetWriteBufferPtr(), i32Size, chpData, i32Size);
	MoveWritePos(i32Size);
	_ui32UseSize += i32Size;
	return i32Size;
}

// ReadPos 에서 데이타 가져옴. ReadPos 이동.
int CRingBuffer::Dequeue(char* chpDest, int i32Size)
{
	int i32UseSize = GetUseSize();
	int i32AbleReadSize = GetNotBrokenReadSize();

	//0. Check PacketSize.
	if (i32Size > i32UseSize)
	{
		i32Size = i32UseSize;
	}

	//1.as if Broken Packet.
	if (i32Size > i32AbleReadSize)
	{
		int i32RemainSize = i32Size - i32AbleReadSize;
		memcpy_s(chpDest, i32AbleReadSize, GetReadBufferPtr(), i32AbleReadSize);
		memcpy_s(chpDest + i32AbleReadSize, i32RemainSize, GetBufferBeginPtr(), i32RemainSize);
		MoveReadPos(i32RemainSize, true);
		_ui32UseSize -= i32Size;
		return i32Size;
	}
	//2. Not Broken Packet.
	memcpy_s(chpDest, i32Size, GetReadBufferPtr(), i32Size);
	MoveReadPos(i32Size);
	_ui32UseSize -= i32Size;
	return i32Size;
}

// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
int CRingBuffer::Peek(char* chpDest, int i32Size)
{
	int i32UseSize = GetUseSize();
	int i32AbleReadSize = GetNotBrokenReadSize();

	//0. 남은 용량보다 많은걸 뽑는다.
	if (i32Size > i32UseSize)
	{
		i32Size = i32UseSize;
	}

	//1. 짤린다.
	if (i32Size > i32AbleReadSize)
	{
		int i32RemainSize = i32Size - i32AbleReadSize;
		memcpy_s(chpDest, i32AbleReadSize, GetReadBufferPtr(), i32AbleReadSize);
		memcpy_s(chpDest + i32AbleReadSize, i32RemainSize, GetBufferBeginPtr(), i32RemainSize);
		return i32Size;
	}
	//2. 안짤린다.
	memcpy_s(chpDest, i32Size, GetReadBufferPtr(), i32Size);
	return i32Size;
}
/********************************************************************************************/


/*Utility************************************************************************************/
// 버퍼가 비었는지 검사.
bool CRingBuffer::IsEmpty(void)
{
	if (GetWriteBufferPtr() == GetReadBufferPtr())
	{
		return true;
	}
	return false;
}

// 버퍼가 가득 찼는지 검사.
bool CRingBuffer::IsFull(void)
{
	char* chpTempWritePtr	= GetWriteBufferPtr();
	char* chpTempReadPtr	= GetReadBufferPtr();

	//만약 WritePtr이 BufferEnd부분을 가리키고 있었다면? WritePos + 1을 해선 안됨.
	if (chpTempWritePtr == _chpBufferEnd)
	{
		//논리적으로 +1 칸인 버퍼의 첫부분이 ReadPtr과 같은지 확인.
		if (chpTempReadPtr == _chpBuffer)
		{
			//같다면 Full.
			return true;
		}
		else
		{
			//아니면 not full.
			return false;
		}
	}

	//일반적인 조건. '쓰기포인터 + 1 == 읽기포인터' 조건성립시 full.
	if ( (chpTempReadPtr) == (chpTempWritePtr + 1) )
	{
		//꽉참.
		return true;
	}
	//꽉안참.
	return false;
}

// WritePtr 이동. (Enqueue할 때 사용).
int CRingBuffer::MoveWritePos(int i32Size, bool bBeginFlag)
{
	if (bBeginFlag == true)
	{
		_chpWritePtr = _chpBuffer;
	}
	_chpWritePtr += i32Size;
	return i32Size;
	/*if (i32Size == 0)
	{
		_chpWritePtr = _chpBuffer;
		return 0;
	}
	else
	{
		_chpWritePtr = _chpWritePtr + i32Size;
	}
	return i32Size;*/
}

// ReadPtr 이동. (데이터 삭제와 동일).
int	CRingBuffer::MoveReadPos(int i32Size, bool bBeginFlag)
{
	if (bBeginFlag == true)
	{
		_chpReadPtr = _chpBuffer;
	}
	_chpReadPtr += i32Size;
	return i32Size;
	/*if (i32Size == 0)
	{
		_chpReadPtr = _chpBuffer;
		return 0;
	}
	else
	{
		_chpReadPtr = _chpReadPtr + i32Size;
	}
	return i32Size;*/
}


// 버퍼의 모든 데이터 삭제. (실제로 삭제할 필요없이, WritePos, ReadPos만 초기값으로 리셋팅하면됨)
void CRingBuffer::ClearBuffer(void)
{
	_chpWritePtr	= _chpBuffer;
	_chpReadPtr		= _chpBuffer;
	return;
}
/********************************************************************************************/
