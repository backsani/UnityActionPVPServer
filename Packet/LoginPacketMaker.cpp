#include "LoginPacketMaker.h"

LoginPacketMaker::LoginPacketMaker()
{
	ConnectionInfo = INIT;
	packetHeader.headerType = ACCEPT;
	memset(UserID, 0, sizeof(UserID)); 
	memset(UserPassword, 0, sizeof(UserPassword)); 
}

int LoginPacketMaker::Serialzed(char* buffer, int size)
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

void LoginPacketMaker::Deserialzed(char* buffer)
{
	int Length = 0;
	Length += UnpackingHeader(buffer);

	memcpy(&ConnectionInfo, buffer + Length, sizeof(ConnectionState));
	Length += sizeof(ConnectionState);

	memcpy(&UserID, buffer + Length, packetHeader.Length - Length);
	Length = packetHeader.Length - Length;

	UserID[Length] = '\0';
	return;
}
