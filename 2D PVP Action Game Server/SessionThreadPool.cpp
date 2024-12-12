#include "SessionThreadPool.h"


SessionThreadPool::SessionThreadPool(size_t numThreads)
{
	for (int i = 0; i < numThreads; i++)
	{
		workers.emplace_back([this]() { workerThread(); });
	}
}

SessionThreadPool::~SessionThreadPool()
{
}

void SessionThreadPool::addTask()
{

}

void SessionThreadPool::workerThread()
{

}
