#pragma once
#include "Packet.h"

class Buffer_Converter {

public:
	Buffer_Converter();

	HeaderType GetHeader(char* buffer);

	~Buffer_Converter();
};