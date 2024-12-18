#pragma once
#pragma comment(lib,"ws2_32")
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <tchar.h>
#include "pch.h"

#include "PacketSDK.h"
#include "MatchManager.h"
//#include "LogManager.h"

/*
패킷 추가시 생성자에서 패킷 버퍼에 추가시켜줘야한다. 또한 PacketSDK에서 해당 헤더를 선언해줘야한다.
*/

extern thread_local std::vector<std::shared_ptr<PacketMaker>> packet;

void initializePacketList();

class LogManager;

class Server
{
	friend class GameSessionManager;

	SOCKET listenSocket = INVALID_SOCKET;

	std::vector<std::shared_ptr<ClientInfo>> mClientInfos;

	std::vector<std::thread> mWorkerThreads;

	std::thread mAccepterThread;

	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;

	std::shared_ptr<MatchManager> mMatchManager;

	std::shared_ptr<GameSessionManager> mGameSessionManager;

	bool mIsWorkerRun = true;

	bool mIsAccepterRun = true;

	char mSocketBuf[1024] = { 0 };

	std::mutex closeSocketMutex;


public:
	std::shared_ptr<LogManager> mLogManager;

	Server();


	bool InitSocket();

	bool BindSocket();

	bool StartServer(const UINT32 maxClientCount);

	void DestroyThread();

	void CreateClient(const UINT32 maxClientCount);

	bool CreateWokerThread();

	bool CreateAccepterThread();

	void WokerThread();

	void AccepterThread();

	std::shared_ptr<ClientInfo> GetEmptyClientInfo();

	bool BindIOCompletionPort(std::shared_ptr<ClientInfo> pClientInfo);

	bool BindRecv(std::shared_ptr<ClientInfo> pClientInfo);

	bool SendMsg(std::shared_ptr<ClientInfo> pClientInfo, char* pMsg, int nLen);

	void CloseSocket(std::shared_ptr<ClientInfo> pClientInfo);

	~Server();
};

