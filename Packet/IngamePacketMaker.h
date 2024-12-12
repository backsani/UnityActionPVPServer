#pragma once
#include "PacketMaker.h"

class IngamePacketMaker : public PacketMaker
{
	ConnectionState ConnectionInfo;

public:

	IngamePacketMaker();

	int Serialzed(char* buffer, int size);
	void Deserialzed(char* buffer);

	void SetConnectionInfo(ConnectionState state) { ConnectionInfo = state; }

	ConnectionState GetConnectionInfo() { return ConnectionInfo; }
};
