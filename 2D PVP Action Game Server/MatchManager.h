#pragma once
#include <queue>
#include <memory>
#include "GameSessionManager.h"

class MatchManager
{
private:
	std::queue<ClientInfo*> MatchingQueue;
	int SessionId = 1;
	std::shared_ptr<GameSessionManager> mGameSessionManager;

public:
	MatchManager(std::shared_ptr<GameSessionManager> sessionManager);

	std::shared_ptr<GameSession> MatchingUser();
	bool AddClientQueue(ClientInfo* client);
};

