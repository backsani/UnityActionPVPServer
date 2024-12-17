#pragma once
#include "Packet.h"

class Buffer_Converter {

public:
	Buffer_Converter();

	HeaderType GetHeader(char* buffer);
	int GetLength(char* buffer);
	int GetSessionId(char* buffer);

	~Buffer_Converter();
};