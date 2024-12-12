#include "GameSession.h"

GameSession::GameSession(int sessionId) : sessionId(sessionId), state(GameState::WAITING) 
{

}

void GameSession::AddClient(ClientInfo* client)
{
	clients.push_back(client);
	if (clients.size() == 2)
	{
		state = GameState::PLAY;
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
