#pragma once
#include "PacketMaker.h"

class LoginPacketMaker : public PacketMaker
{
private:
	char UserID[20];
	char UserPassword[20];
	ConnectionState ConnectionInfo;

public:
	

	LoginPacketMaker();

	int Serialzed(char* buffer, int size);
	void Deserialzed(char* buffer);

	void SetConnectionInfo(ConnectionState state) { ConnectionInfo = state; }

	ConnectionState GetConnectionInfo() { return ConnectionInfo;  }

	const char* GetUserId() const { return UserID; }
};

