#include "GameSessionManager.h"
#include "Server.h"
#include <chrono>
#include "LogManager.h"

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

	for (std::shared_ptr<ClientInfo> client : currentSession->GetClient())
	{
		if (currentState == MATCH_ACCEPT)
		{
			
			mServer->mLogManager->WirteLog(client, "매칭 성공");
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
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		if (SessionList.empty())
		{
			continue;
		}
		for (std::shared_ptr<GameSession> session : SessionList)
		{
			auto copyClients = session->GetClient();
			for (std::shared_ptr<ClientInfo> client : copyClients)
			{

				std::lock_guard<std::mutex> lock(client->mtx);

				if (session->GetState() == READY)
				{
					std::cout << "Pointer address: " << client
						<< ", Object address: " << &(*client)
						<< ", UserId: " << client->GetUserId() << std::endl;


					mThreadPool->addTask([client, &packets, &session, this]()
						{
							int Length = 0;
							int clientLen = client->GetClientInfoLength();

							char* clientInfo = new char[clientLen];
							

							Transform transform;
							transform.xPos = 15*(client->GetsessionUserIndex() % 2);
							transform.yPos = 15 * (client->GetsessionUserIndex() % 2);
							int UserHp = 50;

							{
								client->SetUserTransform(transform);
								client->SetUserHp(UserHp);
							}
							
							client->GetClientInfo(clientInfo);

							packets->SetConnectionInfo(INGAME_INIT);

							char sendBuf[256] = { 0 };

							memcpy(sendBuf + Length, clientInfo, clientLen);
							Length += clientLen;

							packets->SetLength(Length + sizeof(PacketHeader));

							packets->SetUserId(client->GetUserId());

							Length = packets->Serialzed(sendBuf, Length);

							this->GetServer()->SendMsg(client, sendBuf, Length);

							this->GetServer()->BindRecv(client);

							if (++(session->numClientReady) == 2)
								session->SetState(PLAY);

							delete[] clientInfo;
						});
					
				}



				if (session->GetState() == PLAY)
				{
					auto copyClient = session->GetClient();
					for (std::shared_ptr<ClientInfo> clients : copyClient)
					{
						mThreadPool->addTask([clients, client, &packets, this]()
							{
								int Length = 0;
								int clientLen = clients->GetClientInfoLength();

								char* clientInfo = new char[clientLen];
								clients->GetClientInfo(clientInfo);

								packets->SetConnectionInfo(INGAME_MOVE);

								char sendBuf[256] = { 0 };

								memcpy(sendBuf + Length, clientInfo, clientLen);
								Length += clientLen;

								packets->SetLength(Length + sizeof(PacketHeader));

								packets->SetUserId(clients->GetUserId());

								Length = packets->Serialzed(sendBuf, Length);

								this->GetServer()->SendMsg(client, sendBuf, Length);

								this->GetServer()->BindRecv(client);

								delete[] clientInfo;
							});
					}
				}
			}
		}
	}
}

void GameSessionManager::CloseSession(int sessionId, std::shared_ptr<ClientInfo>)

{
	std::shared_ptr<GameSession> currentSession = SessionMap[sessionId];
	if (currentSession == nullptr)
		return;

	std::vector<std::shared_ptr<ClientInfo>> clients = currentSession->GetClient();

	if (clients.size() > 0) {
		clients[0]->SetSessionId(0);
	}
	if (clients.size() > 1) {
		clients[1]->SetSessionId(0);
	}

	// SessionList에서 해당 세션을 삭제
	auto it = std::find(SessionList.begin(), SessionList.end(), currentSession);
	if (it != SessionList.end()) {
		SessionList.erase(it);  // 해당 세션을 리스트에서 제거
	}

	SessionMap.erase(sessionId);

	currentSession.reset();
}
