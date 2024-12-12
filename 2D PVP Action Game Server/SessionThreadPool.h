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
	//생성할 쓰레드의 갯수를 넘겨주면 갯수만큼 쓰레드를 생성한다.
	SessionThreadPool(size_t numThreads);
	~SessionThreadPool();

	//작업 큐에 작업을 추가해주는 함수
	void addTask(const std::function<void()>& task);

private:
	void workerThread();

private:
	//쓰레드들을 저장할 쓰레드벡터
	vector<thread> workers;
	//실행시킬 함수를 저장할 작업큐
	queue<function<void()>> tasks;
	//작업 큐에 락을 걸어줄 뮤텍스
	mutex queueMutex;
	//작업이 있으면 대기중 쓰레드를 깨우기 위한 조건
	condition_variable condition;
	//쓰레드 풀의 실행 상태
	atomic<bool> isRunning;
};

