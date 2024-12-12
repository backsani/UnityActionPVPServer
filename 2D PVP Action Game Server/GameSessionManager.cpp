#include "GameSessionManager.h"

GameSessionManager::GameSessionManager(int numThread)
{
	mThreadPool = std::make_shared<SessionThreadPool>(numThread);
}

GameSessionManager::~GameSessionManager()
{

}

void GameSessionManager::AddSession(std::shared_ptr<GameSession> session)
{
	SessionList.push_back(session);
	SessionMap[session->GetSessionId()] = session;
	//mThreadPool->addTask();
}

void GameSessionManager::RemoveSession(int sessionId)
{
	SessionList.erase(std::find(SessionList.begin(), SessionList.end(), SessionMap[sessionId]), SessionList.end());
	SessionMap.erase(2);
}

void GameSessionManager::ProcessingSessionPacket(int sessionId, PacketMaker* packet)
{
	std::shared_ptr<GameSession> currentSession = SessionMap[sessionId];
	
	for (ClientInfo* client : currentSession->GetClient())
	{
		mThreadPool->addTask([&client, &packet]()
			{
				//TODO: 디시리얼라이즈에 클라이언트 인포가 들어가야 되는거 아닌가. 클라이언트의 정보를 직렬화해서 패킷에 넣어줘야하나..
			});
	}

	
}
