#include "MatchManager.h"

MatchManager::MatchManager(std::shared_ptr<GameSessionManager> sessionManager) : mGameSessionManager(sessionManager)
{

}

std::shared_ptr<GameSession> MatchManager::MatchingUser()
{
	if (MatchingQueue.size() < 2)
		return nullptr;

	auto client1 = MatchingQueue.front();
	MatchingQueue.pop();
	auto client2 = MatchingQueue.front();
	MatchingQueue.pop();

	client1->SetsessionUserIndex(1);
	client2->SetsessionUserIndex(2);

	auto session = std::make_shared<GameSession>(SessionId++);
	session->AddClient(client1);
	session->AddClient(client2);
	mGameSessionManager->AddSession(session);

	return session;
}

bool MatchManager::AddClientQueue(ClientInfo* client)
{
	MatchingQueue.push(client);
	if (MatchingUser())
		return true;
	return false;
}
