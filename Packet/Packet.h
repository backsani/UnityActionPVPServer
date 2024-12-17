#pragma once
#include <string>
#define BUFSIZE 256

enum HeaderType
{
	
	ACCEPT,
	MATCH,
	INGAME
};

struct PacketHeader
{
	int Length;
	HeaderType headerType;
	char userId[20];
};