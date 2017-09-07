#include "SerializationBuffer.hpp"

CPacket::CPacket()
{
	CPacket::Initial();
}

CPacket::CPacket(int iBufferSize)
{
	CPacket::Initial(iBufferSize);
}
/*
CPacket::CPacket(const CPacket &clSrcPacket)
{

}
*/
CPacket::~CPacket()
{
	delete[] m_buf;
}


// ��Ŷ �ʱ�ȭ.
void CPacket::Initial(int iBufferSize)
{
	if (iBufferSize > ePACKET_MAX_SIZE)
		return;
	
	//��Ŷ��������� 1(sizeof(char)) �� ���������ν� �� ��밡���� �뷮�� iBufferSize�� ������.
	m_buf = new char[iBufferSize + 1]; 
	m_bufSize = iBufferSize;
	m_iBufWritePos = 0;
	m_iBufReadPos = 0;
	memset(m_buf, 0, iBufferSize + 1);
	m_buf[iBufferSize] = '\0'; //���۳� null����.
}

int CPacket::ErrCatch(errno_t err)
{
	switch (err)
	{
		case en_ERRNO::PutDataParameterNull:
		{
			fprintf(stdout, "[SubPacket] ERROR : PutData() Parameter is Null.\n");
			break;
		}

		case en_ERRNO::PutDataBufferSizeOver:
		{
			fprintf(stdout, "[SubPacket] ERROR : PutData() Buffer size over.\n");
			break;
		}

		case en_ERRNO::GetDataParameterNull:
		{
			fprintf(stdout, "[SubPacket] ERROR : GetData() Parameter is Null.\n");
			break;
		}
		
		case en_ERRNO::GetDataMemcopyBufferToTemp:
		{
			fprintf(stdout, "[SubPacket] ERROR : GetData() Buffer to Temp memory copy error.\n");
			break;
		}

		case en_ERRNO::GetDataMemcopyTempToDest:
		{
			fprintf(stdout, "[SubPacket] ERROR : GetData() Temp to Dest memory copy error.\n");
			break;
		}

		case en_ERRNO::PutDataMemcopySrcToBuffer:
		{
			fprintf(stdout, "[SubPacket] ERROR : PutData() Source to Buffer memory copy error.\n");
			break;
		}

		case en_ERRNO::GetDataRequestSizeOver:
		{
			fprintf(stdout, "[SubPacket] ERROR : GetData() Request size over then remaining packet size.\n");
			break;
		}

		default:
		{
			fprintf(stdout, "[SubPacket] ERROR : Unknown Error.\n");
			break;
		}
	}
	return -1;
}

//��Ŷ �ı�. (�׳� �ı��� ȣ�� �ع�����)
void CPacket::Release(void)
{
	CPacket::~CPacket();
}

// ��Ŷ û��.
void CPacket::Clear(void)
{
	memset(m_buf, 0, m_bufSize);
	return;
}

// ���� ������ ���.
int	CPacket::GetBufferSize(void)
{
	return m_bufSize;
}

// ��Ŷ ���� �� ���� ������� ���� �� ��Ŷ�� ������ (��밡���� �� ������ ������)
int	CPacket::GetEmptyPacketSize(void)
{
	return (m_bufSize - m_iBufWritePos);
}

// �̹� �����ߴ� ��Ŷ�� �����ϰ�, ���� ������ ��Ŷ ������.  
// ( ���ۿ��ִ� ��Ŷ������ - �̹� ������ ��Ŷ������) -> ( m_iBufWritePos - m_iBufReadPos)
int CPacket::GetRemainPacketSize(void)
{
	return ( m_iBufWritePos - m_iBufReadPos );
}

// ���� ������ ���.
char* CPacket::GetBufferReadPtr(void)
{
	return &m_buf[m_iBufReadPos];
}

char* CPacket::GetBufferZeroPtr(void)
{
	return &m_buf[0];
}

// ���� Pos �̵�. (�����̵��� �ȵ�)
// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
int	CPacket::MoveWritePos(int iSize)
{
	m_iBufWritePos += iSize;
	return m_iBufWritePos;
}
int	CPacket::MoveReadPos(int iSize)
{
	m_iBufReadPos += iSize;
	return m_iBufReadPos;
}

//���ۿ��� ������ ����. (char *)Dest ������. (int)Size. (chpDest�� �޸� �Ҵ��� �Ǿ� �־�� ��)
int	CPacket::GetData(char *chpDest, int iSize)
{
	if(chpDest == nullptr)
		return ErrCatch(-3);

	//�ӽù迭 ���� & �ʱ�ȭ.
	char* temp = new char[iSize + 1];
	memset(temp, 0xff, sizeof(char) * (iSize + 1));
	temp[iSize] = '\0';
	int remainSize = CPacket::GetRemainPacketSize(); //�ִ� ������ �ִ� ��.

	if (iSize <= remainSize) //�����ִ�(�������ִ�)����� �������� ������� ũ�ų� ���� ���
	{
		char* m_buf_ReadPosPtr = CPacket::GetBufferReadPtr();
		//�ӽù迭�� iSize��ŭ ���ۿ��� memcpy_s ī��.
		if (memcpy_s(temp, sizeof(char) * (iSize), m_buf_ReadPosPtr, sizeof(char) * (iSize)))
			return ErrCatch(-4);

		//chpDest�� temp�� ���� ���� ��.
		if (memcpy_s(chpDest, sizeof(char) * (iSize), temp, sizeof(char) * (iSize)))
			return ErrCatch(-5);
		
		MoveReadPos(iSize);
		delete[] temp;
		return iSize;
	}
	else //�������� ����� �����ִ� ������� Ŭ ���. 
	{
		/*!������ �ִ¸�ŭ�� �̴´�? �̰� ���� �ȵ�.
		���ʿ� ���ۿ� 50�����ִµ� 60�޶�� �ϴ°� ��ü�� ��Ŷ����� �߸��� �����.
		�ش� ���̺귯���� �����(������)�� �߸����ɷ� �����ؾ���.!*/
	}	
	return ErrCatch(-7);
}

//���ۿ� ������ ����. (char *)Src ������. (int)SrcSize.
int	CPacket::PutData(char *chpSrc, int iSrcSize)
{
	if (chpSrc == nullptr)
		return ErrCatch(-1);

	//��밡���� �뷮 üũ.
	int iAvailableSize = CPacket::GetEmptyPacketSize();

	if (iSrcSize <= iAvailableSize)
	{
		if (memcpy_s(&m_buf[m_iBufWritePos], sizeof(char) * iSrcSize, chpSrc, sizeof(char) * iSrcSize))
			return ErrCatch(-6);
		MoveWritePos(iSrcSize); //iSrcSize��ŭ WritePos�̵�.
		return iSrcSize;		//�����ϸ� ���� �� ��ŭ ����.
	}
	else //�������� ����� ���� �� �ִ� ������� ū ���.
	{
		/*!���� �� �ִ¸�ŭ�� �ִ´�? �̰͵� ���� �ȵ�.
		�ִٰ� �߰��� ©�� ��Ŷ�� ������ �װ� �ϼ��� �ϳ��� ��Ŷ�� �ƴ�.
		�ϼ����� �ʴ� ��Ŷ�� �׳� �����Ⱚ��. ������ ����. ���� ���� �ʿ䵵 ����.!*/
	}
	return ErrCatch(-2);
}

CPacket& CPacket::operator << (char* cPtrData)
{
	if (cPtrData == nullptr)
		return *this;

	this->PutData(cPtrData, strlen(cPtrData));
	return (*this);
}

/*
byte1 = (int & 0x000000ff)
byte2 = (int & 0x0000ff00) >> 8
byte3 = (int & 0x00ff0000) >> 16
byte4 = (int & 0xff000000) >> 24
*/
CPacket& CPacket::operator << (int& iData)
{
	this->PutData((char*)&iData, sizeof(iData));
	return (*this);
}

CPacket& CPacket::operator << (BYTE& byData)
{
	this->PutData((char*)&byData, sizeof(byData));
	return (*this);
}

CPacket& CPacket::operator << (INT64& i64Data)
{
	this->PutData((char*)&i64Data, sizeof(i64Data));
	return (*this);
}

CPacket& CPacket::operator << (double& dData)
{
	this->PutData((char*)&dData, sizeof(dData));
	return (*this);
}


CPacket& CPacket::operator << (WORD& wdData)
{
	this->PutData((char*)&wdData, sizeof(wdData));
	return (*this);
}

int CPacket::operator >> (int& iData)
{
	int isValid = this->GetData((char*)&iData, sizeof(iData));
	return isValid;
}

int CPacket::operator >> (INT64& i64Data)
{
	int isValid = this->GetData((char*)&i64Data, sizeof(i64Data));
	return isValid;
}

int CPacket::operator >> (double& dData)
{
	int isValid = this->GetData((char*)&dData, sizeof(dData));
	return isValid;
}

int CPacket::operator >> (BYTE& byData)
{
	int isValid = this->GetData((char*)&byData, sizeof(byData));
	return isValid;
}

int CPacket::operator >> (WORD& wdData)
{
	int isValid = this->GetData((char*)&wdData, sizeof(wdData));
	return isValid;
}