#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include "PacketSDK.h"


class SessionThreadPool
{
public:
	//������ �������� ������ �Ѱ��ָ� ������ŭ �����带 �����Ѵ�.
	SessionThreadPool(size_t numThreads);
	~SessionThreadPool();

	//�۾� ť�� �۾��� �߰����ִ� �Լ�
	void addTask(const std::function<void()>& task);

private:
	friend class GameSessionManager;

	void workerThread();

private:
	//��������� ������ �����庤��
	std::vector<std::thread> workers;
	//�����ų �Լ��� ������ �۾�ť
	std::queue<std::function<void()>> tasks;
	//�۾� ť�� ���� �ɾ��� ���ؽ�
	std::mutex queueMutex;
	//�۾��� ������ ����� �����带 ����� ���� ����
	std::condition_variable condition;
	//������ Ǯ�� ���� ����
	std::atomic<bool> isRunning;
};

