#include "GameSessionManager.h"
#include "Server.h"

GameSessionManager::GameSessionManager(Server* server, int numThread)
{
	mServer = server;
	mThreadPool = std::make_shared<SessionThreadPool>(numThread);
}

GameSessionManager::~GameSessionManager()
{

}

void GameSessionManager::AddSession(std::shared_ptr<GameSession> session)
{
	SessionList.push_back(session);
	SessionMap[session->GetSessionId()] = session;
	session->SetState(READY);
	//ProcessingSessionPacket(session->GetSessionId(), );
}

void GameSessionManager::RemoveSession(int sessionId)
{
	SessionList.erase(std::find(SessionList.begin(), SessionList.end(), SessionMap[sessionId]), SessionList.end());
	SessionMap.erase(2);
}

void GameSessionManager::ProcessingSessionPacket(int sessionId, PacketMaker* packet)
{
	std::shared_ptr<GameSession> currentSession = SessionMap[sessionId];
	
	ConnectionState currentState = packet->GetConnectionInfo();

	for (ClientInfo* client : currentSession->GetClient())
	{
		std::cout << "Pointer address: " << client
			<< ", Object address: " << &(*client)
			<< ", UserId: " << client->GetUserId() << std::endl;

		if (currentState == MATCH_ACCEPT)
		{
			mThreadPool->addTask([client, &packet, this]()
				{
					int Length = client->GetClientInfoLength();

					ConnectionState State = packet->GetConnectionInfo();

					char sendBuf[256] = { 0 };

					memcpy(sendBuf, &State, sizeof(State));

					Length = packet->Serialzed(sendBuf, Length);

					this->GetServer()->SendMsg(client, sendBuf, Length);

					this->GetServer()->BindRecv(client);
				});
			
		}
		else if(packet->GetConnectionInfo() == INGAME_MOVE)
		{
			mThreadPool->addTask([&client, &packet, this]()
				{


					int Length = client->GetClientInfoLength();

					char* clientInfo = new char[Length];
					client->GetClientInfo(clientInfo);

					memcpy(client->mRecvBuf, clientInfo, Length);

					Length = packet->Serialzed(client->mRecvBuf, Length);

					this->GetServer()->SendMsg(client, client->mRecvBuf, Length);

				});
		}
		
	}

	
}
