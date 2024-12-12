#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include "PacketSDK.h"

using namespace std;

class SessionThreadPool
{
public:
	//������ �������� ������ �Ѱ��ָ� ������ŭ �����带 �����Ѵ�.
	SessionThreadPool(size_t numThreads);
	~SessionThreadPool();

	//�۾� ť�� �۾��� �߰����ִ� �Լ�
	void addTask(const std::function<void()>& task);

private:
	void workerThread();

private:
	//��������� ������ �����庤��
	vector<thread> workers;
	//�����ų �Լ��� ������ �۾�ť
	queue<function<void()>> tasks;
	//�۾� ť�� ���� �ɾ��� ���ؽ�
	mutex queueMutex;
	//�۾��� ������ ����� �����带 ����� ���� ����
	condition_variable condition;
	//������ Ǯ�� ���� ����
	atomic<bool> isRunning;
};

