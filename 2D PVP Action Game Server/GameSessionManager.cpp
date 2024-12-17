#include "GameSessionManager.h"
#include "Server.h"
#include <chrono>

GameSessionManager::GameSessionManager(Server* server, int numThread)
{
	mServer = server;
	mThreadPool = std::make_shared<SessionThreadPool>(numThread);

	mThreadPool->workers.emplace_back([this]() { SessionUpdate(); });

}

GameSessionManager::~GameSessionManager()
{

}

void GameSessionManager::AddSession(std::shared_ptr<GameSession> session)
{
	SessionList.push_back(session);
	SessionMap[session->GetSessionId()] = session;
}

void GameSessionManager::RemoveSession(int sessionId)
{
	SessionList.erase(std::find(SessionList.begin(), SessionList.end(), SessionMap[sessionId]), SessionList.end());
	SessionMap.erase(2);
}

void GameSessionManager::ProcessingSessionPacket(int sessionId, ConnectionState state)
{
	auto packets = static_cast<MatchPacketMaker*>(packet[MATCH].get());

	std::shared_ptr<GameSession> currentSession = SessionMap[sessionId];
	
	ConnectionState currentState = state;

	for (ClientInfo* client : currentSession->GetClient())
	{
		if (currentState == MATCH_ACCEPT)
		{
			mThreadPool->addTask([client, &packets, this]()
				{
					int Length = 0;

					ConnectionState State = MATCH_ACCEPT;

					char sendBuf[256] = { 0 };

					memcpy(sendBuf, &State, sizeof(State));

					Length += sizeof(State);

					Length = packets->Serialzed(sendBuf, Length);



					this->GetServer()->SendMsg(client, sendBuf, Length);

					this->GetServer()->BindRecv(client);
				});

			currentSession->SetState(READY);
			
		}
				
	}

	
}

void GameSessionManager::SessionUpdate()
{
	initializePacketList();

	auto packets = static_cast<IngamePacketMaker*>(packet[INGAME].get());

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if (SessionList.empty())
		{
			continue;
		}
		for (std::shared_ptr<GameSession> session : SessionList)
		{
			int numClientsSent = 0;

			for (ClientInfo* client : session->GetClient())
			{

				if (session->GetState() == READY)
				{
					std::cout << "Pointer address: " << client
						<< ", Object address: " << &(*client)
						<< ", UserId: " << client->GetUserId() << std::endl;


					mThreadPool->addTask([client, &packets, this]()
						{
							int Length = 0;
							int clientLen = client->GetClientInfoLength();

							char* clientInfo = new char[clientLen];
							client->GetClientInfo(clientInfo);

							Transform transform;
							transform.xPos = 15;
							transform.yPos = 15;
							client->SetUserTransform(transform);

							packets->SetConnectionInfo(INGAME_INIT);

							char sendBuf[256] = { 0 };

							memcpy(sendBuf + Length, clientInfo, clientLen);
							Length += clientLen;

							packets->SetLength(Length + sizeof(PacketHeader));

							packets->SetUserId(client->GetUserId());

							Length = packets->Serialzed(sendBuf, Length);

							this->GetServer()->SendMsg(client, sendBuf, Length);

							this->GetServer()->BindRecv(client);

							delete[] clientInfo;
						});
					if(++numClientsSent == 2)
						session->SetState(PLAY);
				}

				if (session->GetState() == PLAY)
				{
					for (ClientInfo* client : session->GetClient())
					{

					}
				}
			}
		}
	}
}
