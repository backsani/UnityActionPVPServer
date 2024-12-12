#include "PacketMaker.h"

int PacketMaker::UnpackingHeader(char* buffer)
{
	int Length = 0;
	memcpy(&packetHeader.Length, buffer, sizeof(packetHeader.Length));
	Length += sizeof(packetHeader.Length);

	memcpy(&packetHeader.headerType, buffer + Length, sizeof(packetHeader.headerType));
	Length += sizeof(packetHeader.headerType);

	memcpy(&packetHeader.userId, buffer + Length, sizeof(packetHeader.userId));
	Length += sizeof(packetHeader.userId);

	return Length;
}

int PacketMaker::PackingHeader(char* buffer)
{
	char header[sizeof(packetHeader)];

	int Length = 0;
	memcpy(header, &packetHeader.Length, sizeof(packetHeader.Length));
	Length += sizeof(packetHeader.Length);

	memcpy(header + Length, &packetHeader.headerType, sizeof(packetHeader.headerType));
	Length += sizeof(packetHeader.headerType);

	memcpy(header + Length, &packetHeader.userId, sizeof(packetHeader.userId));
	Length += sizeof(packetHeader.userId);

	memcpy(buffer, header, Length);

	return Length;
}
