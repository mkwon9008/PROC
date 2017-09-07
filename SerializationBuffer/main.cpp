#include "SerializationBuffer.hpp"

//testSET.
int main(void)
{
	CPacket packet(50);

	int number = 7795684;
	INT64 uniqueNum = 884657993212456;
	char* name = "kwon";
	double pi = 3.141592;
	LONGLONG longNum = 5325126156;
	WORD wordVal = 777;
	BYTE byData = 0xee;

	packet << number;
	packet << uniqueNum;
	packet << name;
	packet << pi;
	packet << longNum;
	packet << wordVal;
	packet << byData;

	int remain = packet.GetRemainPacketSize();
	int sumSize = sizeof(number) + sizeof(uniqueNum) + strlen(name) + sizeof(pi) 
				+ sizeof(longNum) + sizeof(wordVal) + sizeof(byData);

	int number2;
	INT64 uniqueNum2;
	char name2[4];
	double pi2;
	LONGLONG longNum2;
	WORD wordVal2;
	BYTE byData2;

	packet >> number2;
	packet >> uniqueNum2;
	packet.GetData(name2, 4);
	packet >> pi2;
	packet >> longNum2;
	packet >> wordVal2;
	packet >> byData2;

	remain = packet.GetRemainPacketSize();

	return 0;
}
