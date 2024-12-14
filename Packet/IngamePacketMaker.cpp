#include "IngamePacketMaker.h"

IngamePacketMaker::IngamePacketMaker()
{
	ConnectionInfo = INIT;
	packetHeader.headerType = INGAME;
}

int IngamePacketMaker::Serialzed(char* buffer, int size)
{
	char* data = new char[sizeof(PacketHeader) + sizeof(ConnectionInfo) + size];
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

	memcpy(&buffer, buffer + Length, strlen(buffer) - Length);
	Length += strlen(buffer) - Length;

	buffer[Length] = '\0';

	return;
}
