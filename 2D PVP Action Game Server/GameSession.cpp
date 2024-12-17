#include "GameSession.h"

GameSession::GameSession(int sessionId) : sessionId(sessionId), mState(GameState::WAITING) , numClientReady(0)
{

}

void GameSession::AddClient(std::shared_ptr<ClientInfo> client)
{
	client->SetSessionId(GetSessionId());
	clients.push_back(client);
	if (clients.size() == 2)
	{
		PrintSessionInfo();
	}
}

void GameSession::RemoveClient(std::shared_ptr<ClientInfo> client)
{

}
