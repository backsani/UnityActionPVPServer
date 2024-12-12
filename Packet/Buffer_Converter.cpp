#define _CRT_SECURE_NO_WARNINGS
#include "Buffer_Converter.h"
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