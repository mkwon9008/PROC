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


// 패킷 초기화.
void CPacket::Initial(int iBufferSize)
{
	if (iBufferSize > ePACKET_MAX_SIZE)
		return;
	
	//패킷헤더사이즈 1(sizeof(char)) 을 더해줌으로써 실 사용가능한 용량을 iBufferSize로 맞춰줌.
	m_buf = new char[iBufferSize + 1]; 
	m_bufSize = iBufferSize;
	m_iBufWritePos = 0;
	m_iBufReadPos = 0;
	memset(m_buf, 0, iBufferSize + 1);
	m_buf[iBufferSize] = '\0'; //버퍼끝 null삽입.
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

//패킷 파괴. (그냥 파괴자 호출 해버리자)
void CPacket::Release(void)
{
	CPacket::~CPacket();
}

// 패킷 청소.
void CPacket::Clear(void)
{
	memset(m_buf, 0, m_bufSize);
	return;
}

// 버퍼 사이즈 얻기.
int	CPacket::GetBufferSize(void)
{
	return m_bufSize;
}

// 패킷 생성 후 아직 사용하지 않은 빈 패킷의 사이즈 (사용가능한 빈 버퍼의 사이즈)
int	CPacket::GetEmptyPacketSize(void)
{
	return (m_bufSize - m_iBufWritePos);
}

// 이미 추출했던 패킷은 제외하고, 추출 가능한 패킷 사이즈.  
// ( 버퍼에있는 패킷사이즈 - 이미 추출한 패킷사이즈) -> ( m_iBufWritePos - m_iBufReadPos)
int CPacket::GetRemainPacketSize(void)
{
	return ( m_iBufWritePos - m_iBufReadPos );
}

// 버퍼 포인터 얻기.
char* CPacket::GetBufferReadPtr(void)
{
	return &m_buf[m_iBufReadPos];
}

char* CPacket::GetBufferZeroPtr(void)
{
	return &m_buf[0];
}

// 버퍼 Pos 이동. (음수이동은 안됨)
// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
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

//버퍼에서 데이터 빼기. (char *)Dest 포인터. (int)Size. (chpDest는 메모리 할당이 되어 있어야 함)
int	CPacket::GetData(char *chpDest, int iSize)
{
	if(chpDest == nullptr)
		return ErrCatch(-3);

	//임시배열 선언 & 초기화.
	char* temp = new char[iSize + 1];
	memset(temp, 0xff, sizeof(char) * (iSize + 1));
	temp[iSize] = '\0';
	int remainSize = CPacket::GetRemainPacketSize(); //최대 뽑을수 있는 수.

	if (iSize <= remainSize) //남아있는(뽑을수있는)사이즈가 뽑으려는 사이즈보다 크거나 같을 경우
	{
		char* m_buf_ReadPosPtr = CPacket::GetBufferReadPtr();
		//임시배열에 iSize만큼 버퍼에서 memcpy_s 카피.
		if (memcpy_s(temp, sizeof(char) * (iSize), m_buf_ReadPosPtr, sizeof(char) * (iSize)))
			return ErrCatch(-4);

		//chpDest에 temp의 값을 덮어 씀.
		if (memcpy_s(chpDest, sizeof(char) * (iSize), temp, sizeof(char) * (iSize)))
			return ErrCatch(-5);
		
		MoveReadPos(iSize);
		delete[] temp;
		return iSize;
	}
	else //뽑으려는 사이즈가 남아있는 사이즈보다 클 경우. 
	{
		/*!뽑을수 있는만큼만 뽑는다? 이건 말이 안됨.
		애초에 버퍼에 50갖고있는데 60달라고 하는거 자체가 패킷계산이 잘못된 경우임.
		해당 라이브러리의 사용자(개발자)가 잘못쓴걸로 생각해야함.!*/
	}	
	return ErrCatch(-7);
}

//버퍼에 데이터 삽입. (char *)Src 포인터. (int)SrcSize.
int	CPacket::PutData(char *chpSrc, int iSrcSize)
{
	if (chpSrc == nullptr)
		return ErrCatch(-1);

	//사용가능한 용량 체크.
	int iAvailableSize = CPacket::GetEmptyPacketSize();

	if (iSrcSize <= iAvailableSize)
	{
		if (memcpy_s(&m_buf[m_iBufWritePos], sizeof(char) * iSrcSize, chpSrc, sizeof(char) * iSrcSize))
			return ErrCatch(-6);
		MoveWritePos(iSrcSize); //iSrcSize만큼 WritePos이동.
		return iSrcSize;		//성공하면 넣은 수 만큼 리턴.
	}
	else //넣으려는 사이즈가 넣을 수 있는 사이즈보다 큰 경우.
	{
		/*!넣을 수 있는만큼만 넣는다? 이것도 말이 안됨.
		넣다가 중간에 짤린 패킷을 버리면 그건 완성된 하나의 패킷이 아님.
		완성되지 않는 패킷은 그냥 쓰레기값임. 쓸데가 없음. 따라서 넣을 필요도 없다.!*/
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