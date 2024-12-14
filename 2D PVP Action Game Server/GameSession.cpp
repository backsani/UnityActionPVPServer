#include "GameSession.h"

GameSession::GameSession(int sessionId) : sessionId(sessionId), mState(GameState::WAITING) 
{

}

void GameSession::AddClient(ClientInfo* client)
{
	clients.push_back(client);
	if (clients.size() == 2)
	{
		PrintSessionInfo();
	}
}

void GameSession::RemoveClient(SOCKET playerId)
{

}

void GameSession::Test()
{
	for (ClientInfo* client : clients)
	{
		
	}
}
