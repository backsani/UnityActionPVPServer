#include "MatchManager.h"

MatchManager::MatchManager(std::shared_ptr<GameSessionManager> sessionManager) : mGameSessionManager(sessionManager)
{

}

std::shared_ptr<GameSession> MatchManager::MatchingUser()
{
	if (MatchingQueue.size() < 2)
		return 0;

	std::shared_ptr<ClientInfo> client1 = MatchingQueue.front();
	MatchingQueue.pop();
	std::shared_ptr<ClientInfo> client2 = MatchingQueue.front();
	MatchingQueue.pop();

	client1->SetsessionUserIndex(0);
	client2->SetsessionUserIndex(1);

	std::shared_ptr<GameSession> session = std::make_shared<GameSession>(SessionId++);
	session->AddClient(client1);
	session->AddClient(client2);
	mGameSessionManager->AddSession(session);

	return session;
}

std::shared_ptr<GameSession> MatchManager::AddClientQueue(std::shared_ptr<ClientInfo> client)
{
	MatchingQueue.push(client);
	
	return MatchingUser();
}
