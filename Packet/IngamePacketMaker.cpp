#include "IngamePacketMaker.h"

IngamePacketMaker::IngamePacketMaker()
{
	ConnectionInfo = INIT;
	packetHeader.headerType = INGAME;
}

int IngamePacketMaker::Serialzed(char* buffer, int size)
{
	char* data = new char[size + sizeof(PacketHeader)];
	int Length = 0;
	Length = PackingHeader(data);

	memcpy(data + Length, &ConnectionInfo, sizeof(ConnectionInfo));
	Length += sizeof(ConnectionInfo);

	memcpy(data + Length, buffer, strlen(buffer));
	Length += strlen(buffer);

	memcpy(buffer, data, Length);

	return Length;
}

void IngamePacketMaker::Deserialzed(char* buffer)
{
	int Length = 0;
	Length += UnpackingHeader(buffer);

	memcpy(&ConnectionInfo, buffer + Length, sizeof(ConnectionState));
	Length += sizeof(ConnectionState);

	

	
	return;
}
