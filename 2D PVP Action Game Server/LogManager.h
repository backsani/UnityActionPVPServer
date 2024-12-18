#pragma once
#include <iostream>
#include <tchar.h>
#include "Server.h"

class ClientInfo;

class LogManager
{
	HANDLE hFile;
	int offset;
	DWORD dwWroteBytes;

public:
	LogManager();

	void WirteLog(std::shared_ptr<ClientInfo> client, std::string Log);
	
	~LogManager();
};

