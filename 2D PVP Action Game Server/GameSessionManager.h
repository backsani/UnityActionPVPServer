#pragma once
#include <vector>
#include <map>
#include <memory>
#include "GameSession.h"
#include "SessionThreadPool.h"



class GameSessionManager
{
private:
	//게임 세션을 저장할 벡터
	std::vector<std::shared_ptr<GameSession>> SessionList;
	//게임 세션과 세션 ID를 맵핑해주는 Map
	std::map<int, std::shared_ptr<GameSession>> SessionMap;
	std::shared_ptr<SessionThreadPool> mThreadPool;

public:
	GameSessionManager(int numThread);
	~GameSessionManager();

	//Add, Remove 둘 다 뮤텍스가 필요할 수도 있다.
	//세션이 추가 시 vector에 넣고 맵핑을 수행하는 함수
	void AddSession(std::shared_ptr<GameSession> session);
	//세션 만료시 vector와 map의 키를 지우는 함수
	void RemoveSession(int sessionId);
	std::shared_ptr<SessionThreadPool> GetSessionThreadPool() { return mThreadPool; }

	//TODO: 매개변수로 패킷 넘기기
	void ProcessingSessionPacket(int sessionId, PacketMaker* paket);
};

