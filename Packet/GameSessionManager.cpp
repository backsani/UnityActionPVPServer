#include "GameSessionManager.h"

GameSessionManager::GameSessionManager()
{

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
