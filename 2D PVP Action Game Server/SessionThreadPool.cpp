#include "SessionThreadPool.h"


SessionThreadPool::SessionThreadPool(size_t numThreads) : isRunning(true)
{
	for (int i = 0; i < numThreads; i++)
	{
		workers.emplace_back([this]() { workerThread(); });
	}
}

SessionThreadPool::~SessionThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isRunning = false;
    }
    condition.notify_all(); // ��� �����带 ����
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void SessionThreadPool::addTask(const std::function<void()>& task)
{
	{
		std::lock_guard<std::mutex> lock(queueMutex);
		tasks.push(task);
	}
	condition.notify_one(); // ��� ���� ������ �� �ϳ��� ����
}

void SessionThreadPool::workerThread()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return !isRunning || !tasks.empty(); });

            if (!isRunning && tasks.empty()) {
                return; // ������ ���� ����
            }

            task = tasks.front(); // �۾� ��������
            tasks.pop();
        }
        task(); // �۾� ����
    }
}
