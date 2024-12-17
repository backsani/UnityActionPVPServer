#define _CRT_SECURE_NO_WARNINGS
#include "Buffer_Converter.h"
#include "PacketMaker.h"
#include <string.h>

Buffer_Converter::Buffer_Converter() {

}
Buffer_Converter::~Buffer_Converter() {

}

HeaderType Buffer_Converter::GetHeader(char* buffer)
{
	HeaderType header;
	memcpy(&header, buffer + 4, sizeof(HeaderType));

	return header;
}

int Buffer_Converter::GetSessionId(char* buffer)
{
	int sessionId = 0;
	int readPoint = sizeof(PacketHeader) + sizeof(ConnectionState);
	memcpy(&sessionId, buffer + readPoint, sizeof(HeaderType));

	return sessionId;
}

int Buffer_Converter::GetLength(char* buffer)
{
	int length = 0;

	memcpy(&length, buffer, sizeof(length));

	return length;
}