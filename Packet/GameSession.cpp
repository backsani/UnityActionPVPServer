#include "GameSession.h"

void GameSession::AddClient(ClientInfo* client)
{
	clients.push_back(client);
	if (clients.size() == 2)
	{
		state = GameState::PLAY;
	}
}

void GameSession::RemoveClient(SOCKET playerId)
{

}
