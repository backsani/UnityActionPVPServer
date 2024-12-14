#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "ClientInfo.h"


enum GameState
{
	WAITING,
	READY,
	PLAY,
	COMPLETED
};

class GameSession
{
private:
	int sessionId;
	std::vector<ClientInfo*> clients;
	GameState mState;

public:
	GameSession(int sessionId); 

	void AddClient(ClientInfo* client);
	void RemoveClient(SOCKET playerId);

	void Test();

	GameState GetState() const { return mState; }
	int GetSessionId() { return sessionId; }
	std::vector<ClientInfo*>& GetClient() { return clients; }

	void SetState(GameState state) { mState = state; }

	void PrintSessionInfo() const {
		std::cout << "Session ID: " << sessionId << "\nPlayers: ";
		for (const auto& client : clients) {
			std::cout << client->GetUserId() << " ";
		}
		std::cout << "\nState: " << static_cast<int>(mState) << std::endl;
	}
};

