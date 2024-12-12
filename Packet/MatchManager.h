#pragma once
#include <queue>
#include <memory>
#include "ClientInfo.h"
#include "GameSession.h"

class MatchManager
{
private:
	std::queue<ClientInfo*> MatchingQueue;
	int SessionId = 1;

public:
	std::shared_ptr<GameSession> MatchingUser();
	void AddClientQueue(ClientInfo* client);
};

