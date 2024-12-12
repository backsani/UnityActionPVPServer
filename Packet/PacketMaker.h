#pragma once
#include "pch.h"
#include "Packet.h"

enum ConnectionState
{
	INIT,
	LOGIN,
	SIGNUP,
	LOGIN_SUCCESS,
	SIGNUP_SUCCESS,
	LOGIN_FAIL,
	SIGNUP_FAIL,
	MATCH_REQUEST,
	MATCH_FIND,
	MATCH_ACCEPT,
	MATCH_REFUSE
};

class PacketMaker
{
protected:
	PacketHeader packetHeader;

public:
	virtual int Serialzed(char* buffer, int size) = 0;
	virtual void Deserialzed(char* buffer) = 0;

protected:
	int UnpackingHeader(char* buffer);
	int PackingHeader(char* buffer);
};