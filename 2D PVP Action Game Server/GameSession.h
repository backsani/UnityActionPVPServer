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
	std::vector<std::shared_ptr<ClientInfo>> clients;
	GameState mState;

public:
	int numClientReady;

	GameSession(int sessionId); 

	void AddClient(std::shared_ptr<ClientInfo> client);
	void RemoveClient(std::shared_ptr<ClientInfo> client);

	GameState GetState() const { return mState; }
	int GetSessionId() { return sessionId; }
	std::vector<std::shared_ptr<ClientInfo>>& GetClient() { return clients; }

	void SetState(GameState state) { mState = state; }

	void PrintSessionInfo() const {
		std::cout << "Session ID: " << sessionId << "\nPlayers: ";
		for (const std::shared_ptr<ClientInfo>& client : clients) {
			std::cout << client->GetUserId() << " ";
		}
		std::cout << "\nState: " << static_cast<int>(mState) << std::endl;
	}

};

