#pragma once
#include <queue>
#include <memory>
#include "GameSessionManager.h"

class MatchManager
{
private:
	std::queue<std::shared_ptr<ClientInfo>> MatchingQueue;
	int SessionId = 0;
	std::shared_ptr<GameSessionManager> mGameSessionManager;

public:
	MatchManager(std::shared_ptr<GameSessionManager> sessionManager);

	std::shared_ptr<GameSession> MatchingUser();
	std::shared_ptr<GameSession> AddClientQueue(std::shared_ptr<ClientInfo> client);
};

