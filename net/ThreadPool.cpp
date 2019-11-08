#include "ThreadPool.h"

using namespace hhl;

ThreadPool::ThreadPool(const std::string & nameArg)
	: mutex_(),
	notEmpty_(mutex_),
	notFull_(mutex_),
	name_(nameArg),
	maxQueueSize_(0),
	running_(false)
{


}

void ThreadPool::start(int numThreads)
{
	assert(threads_.empty());
	running_ = true;
	threads_.reserve(numThreads);
	for (int i = 0; i < numThreads; i++)
	{
		char id[32];
		snprintf(id, sizeof(id), "%d", ++i);
		//threads_.emplace_back(new hhl::thread(


		//))

	}


}
