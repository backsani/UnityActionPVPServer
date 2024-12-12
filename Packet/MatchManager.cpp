#include "MatchManager.h"

std::shared_ptr<GameSession> MatchManager::MatchingUser()
{
	if (MatchingQueue.size() < 2)
		return nullptr;

	auto client1 = MatchingQueue.front();
	MatchingQueue.pop();
	auto client2 = MatchingQueue.front();
	MatchingQueue.pop();

	auto session = std::make_shared<GameSession>(SessionId++);
	session->AddClient(client1);
	session->AddClient(client1);

	return session;
}

void MatchManager::AddClientQueue(ClientInfo* client)
{
	MatchingQueue.push(client);
	MatchingUser();
}
