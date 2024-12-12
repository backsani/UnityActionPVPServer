#pragma once
#include <vector>
#include <map>
#include "GameSession.h"
#include "SessionThreadPool.h"

class GameSessionManager
{
private:
	//���� ������ ������ ����
	std::vector<std::shared_ptr<GameSession>> SessionList;
	//���� ���ǰ� ���� ID�� �������ִ� Map
	std::map<int, std::shared_ptr<GameSession>> SessionMap;

public:
	GameSessionManager();
	~GameSessionManager();

	//Add, Remove �� �� ���ؽ��� �ʿ��� ���� �ִ�.
	//������ �߰� �� vector�� �ְ� ������ �����ϴ� �Լ�
	void AddSession(std::shared_ptr<GameSession> session);
	//���� ����� vector�� map�� Ű�� ����� �Լ�
	void RemoveSession(int sessionId);
};

