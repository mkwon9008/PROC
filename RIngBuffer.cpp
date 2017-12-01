#include "RingBuffer.hpp"

CRingBuffer::CRingBuffer(int i32BufferSize)
	: _ui32BufferSize(i32BufferSize), _ui32UseSize(0),
	_chpBuffer(nullptr), _chpWritePtr(nullptr), _chpReadPtr(nullptr), _chpBufferEnd(nullptr)
{
	//Ŭ���� ������.
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

//�ʱ�ȭ �Լ�. (Param : ���ۻ������� -1 ���� �־ �ش� ������ ���� ����Ű�� �ϴ� const BufferEnd����.)
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
// ������ �� ������ ���.
int	CRingBuffer::GetBufferSize(void)
{
	return _ui32BufferSize;
}

// ���� ������� �뷮 ���.
int	CRingBuffer::GetUseSize(void)
{
	return _ui32UseSize;
}

// ���� ���ۿ� ���� �뷮 ���.
int	CRingBuffer::GetFreeSize(void)
{
	return (_ui32BufferSize) - (_ui32UseSize);
}

// ������ Rear�κ�. ���� �κ��� ������.
char* CRingBuffer::GetWriteBufferPtr(void)
{
	return _chpWritePtr;
}

// ������ Front�κ�. �д� �κ��� ������.
char* CRingBuffer::GetReadBufferPtr(void)
{
	return _chpReadPtr;
}

// ���� �����ͷ� �ܺο��� �ѹ濡 �� �� �ִ� ����.
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

// ���� �����ͷ� �ܺο��� �ѹ濡 ���� �� �ִ� ����.
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

// ������ ù ���ۺκ� ������.
char* CRingBuffer::GetBufferBeginPtr(void)
{
	return _chpBuffer;
}
/********************************************************************************************/



/*Funtion************************************************************************************/
// WritePos �� ����Ÿ ����.
//chpData:�������� ������, i32Size : �������� �������� ��, return : �ֱ⼺���� ��.
int CRingBuffer::Enqueue(char* chpData, int i32Size)
{
	int i32AbleWriteSize = GetNotBrokenWriteSize();
	int i32FreeSize = GetFreeSize();

	//1. �������� ������ > ��밡���� ������. (�����ѵ������� �ִ´�.)
	if (i32Size > i32FreeSize)
	{
		i32Size = i32FreeSize;
	}
	
	//2. ����� �κ��� �ִ�.
	if (i32Size > i32AbleWriteSize)
	{
		//RemainSize(����µ����� �ְ� ���¿뷮) = Size(�������¿뷮) - AbleWriteSize(����µ����� �뷮);
		int i32RemainSize = i32Size - i32AbleWriteSize;

		//memcpy�� �ι� �ؾ���. �ϴ� ����µ����� ����. (WritePos���� ������ ����ºκб���)
		memcpy_s(GetWriteBufferPtr(), i32AbleWriteSize, chpData, i32AbleWriteSize);
		
		//�ι�° memcpy. ����� ���� �����ŭ ����. (������ ù�κ� ���� ���� ���������)
		memcpy_s(GetBufferBeginPtr(), i32RemainSize, chpData + i32AbleWriteSize, i32RemainSize);

		//WritePos�̵�. (������ ù�κп��� ReaminSize��ŭ)
		MoveWritePos(i32RemainSize, true);

		//������� �뷮 ���.
		_ui32UseSize += i32Size;

		//������ ����.
		return i32Size;
	}
	
	//3. ����� �κ��� ����.
	memcpy_s(GetWriteBufferPtr(), i32Size, chpData, i32Size);
	MoveWritePos(i32Size);
	_ui32UseSize += i32Size;
	return i32Size;
}

// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
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

// ReadPos ���� ����Ÿ �о��. ReadPos ����.
int CRingBuffer::Peek(char* chpDest, int i32Size)
{
	int i32UseSize = GetUseSize();
	int i32AbleReadSize = GetNotBrokenReadSize();

	//0. ���� �뷮���� ������ �̴´�.
	if (i32Size > i32UseSize)
	{
		i32Size = i32UseSize;
	}

	//1. ©����.
	if (i32Size > i32AbleReadSize)
	{
		int i32RemainSize = i32Size - i32AbleReadSize;
		memcpy_s(chpDest, i32AbleReadSize, GetReadBufferPtr(), i32AbleReadSize);
		memcpy_s(chpDest + i32AbleReadSize, i32RemainSize, GetBufferBeginPtr(), i32RemainSize);
		return i32Size;
	}
	//2. ��©����.
	memcpy_s(chpDest, i32Size, GetReadBufferPtr(), i32Size);
	return i32Size;
}
/********************************************************************************************/


/*Utility************************************************************************************/
// ���۰� ������� �˻�.
bool CRingBuffer::IsEmpty(void)
{
	if (GetWriteBufferPtr() == GetReadBufferPtr())
	{
		return true;
	}
	return false;
}

// ���۰� ���� á���� �˻�.
bool CRingBuffer::IsFull(void)
{
	char* chpTempWritePtr	= GetWriteBufferPtr();
	char* chpTempReadPtr	= GetReadBufferPtr();

	//���� WritePtr�� BufferEnd�κ��� ����Ű�� �־��ٸ�? WritePos + 1�� �ؼ� �ȵ�.
	if (chpTempWritePtr == _chpBufferEnd)
	{
		//�������� +1 ĭ�� ������ ù�κ��� ReadPtr�� ������ Ȯ��.
		if (chpTempReadPtr == _chpBuffer)
		{
			//���ٸ� Full.
			return true;
		}
		else
		{
			//�ƴϸ� not full.
			return false;
		}
	}

	//�Ϲ����� ����. '���������� + 1 == �б�������' ���Ǽ����� full.
	if ( (chpTempReadPtr) == (chpTempWritePtr + 1) )
	{
		//����.
		return true;
	}
	//�˾���.
	return false;
}

// WritePtr �̵�. (Enqueue�� �� ���).
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

// ReadPtr �̵�. (������ ������ ����).
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


// ������ ��� ������ ����. (������ ������ �ʿ����, WritePos, ReadPos�� �ʱⰪ���� �������ϸ��)
void CRingBuffer::ClearBuffer(void)
{
	_chpWritePtr	= _chpBuffer;
	_chpReadPtr		= _chpBuffer;
	return;
}
/********************************************************************************************/
