#include "ThreadPool.h"
#include "exception"

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

hhl::ThreadPool::~ThreadPool()
{
	if (running_)
	{
		stop();
	}
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
		threads_.emplace_back(new hhl::thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
		threads_[i]->start();
	}
	if (numThreads == 0 && threadInitCallback_)
	{
		threadInitCallback_();
	}

}

void hhl::ThreadPool::run(Task f)
{
	if (threads_.empty())
	{
		f();
	}
	else
	{
		MutexLockGuard lock(mutex_);
		while (isFull())
		{
			notFull_.wait();
		}
		assert(!isFull());
		queue_.push_back(f);
		notEmpty_.notify();
	}
}

void hhl::ThreadPool::stop()
{
	{
		{
			MutexLockGuard lock(mutex_);
			running_ = false;
			notEmpty_.notifyAll();
		}
		for (auto & thr : threads_)
		{
			thr->join();
		}
	}
}

size_t hhl::ThreadPool::queueSize() const
{
	MutexLockGuard lock(mutex_);
	return queue_.size();
}

bool hhl::ThreadPool::isFull()
{
	mutex_.assertLocked();
	return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void hhl::ThreadPool::runInThread()
{
	try
	{
		if(threadInitCallback_)
		{
			threadInitCallback_();
		}
		while (running_)
		{
			Task task(take());
			if (task)
			{
				task();
			}
		}

	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason: %s\n", ex.what());
		//fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
		abort();
	}
	catch (...)
	{
		fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
		throw; // rethrow
	}
}

ThreadPool::Task hhl::ThreadPool::take()
{
	MutexLockGuard lock(mutex_);
	while (queue_.empty() && running_)
	{
		notEmpty_.wait();
	}

	Task task;
	if (!queue_.empty())
	{
		task = queue_.front();
		queue_.pop_front();
		if (maxQueueSize_ > 0)
		{
			notFull_.notify();
		}
	}
	return task;
}
