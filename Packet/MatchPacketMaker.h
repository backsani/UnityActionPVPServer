#pragma once
#include "PacketMaker.h"


class MatchPacketMaker : public PacketMaker
{
	ConnectionState ConnectionInfo;

public:

	MatchPacketMaker();

	int Serialzed(char* buffer, int size);
	void Deserialzed(char* buffer);

	void SetConnectionInfo(ConnectionState state) { ConnectionInfo = state; }

	ConnectionState GetConnectionInfo() { return ConnectionInfo; }
};

