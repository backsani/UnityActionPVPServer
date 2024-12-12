#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "ClientInfo.h"


enum GameState
{
	WAITING,
	PLAY,
	COMPLETED
};

class GameSession
{
private:
	int sessionId;
	std::vector<ClientInfo*> clients;
	GameState state;

public:
	GameSession(int sessionId) : sessionId(sessionId), state(GameState::WAITING){}

	void AddClient(ClientInfo* client);
	void RemoveClient(SOCKET playerId);

	GameState GetState() const { return state; }
	int GetSessionId() { return sessionId; }

	void PrintSessionInfo() const {
		std::cout << "Session ID: " << sessionId << "\nPlayers: ";
		for (const auto& client : clients) {
			std::cout << client->GetUserId() << " ";
		}
		std::cout << "\nState: " << static_cast<int>(state) << std::endl;
	}
};

