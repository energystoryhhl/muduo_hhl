#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "src/Condition.h"
#include "src/mutex.h"
#include "src/thread.h"
#include "base/types.h"
#include "noncopyable.h"

#include <deque>
#include <vector>
#include <memory>

namespace hhl
{
	class ThreadPool :noncopyable
	{
	public:
		typedef std::function<void()> Task;

		explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
		~ThreadPool();

		void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
		void setThreadInitCallback(const Task& cb)
		{
			threadInitCallback_ = cb;
		}

		void start(int numThreads);


	private:

		mutable	MutexLock		mutex_;
		Condition				notEmpty_;
		Condition				notFull_;
		std::string				name_;
		Task					threadInitCallback_;
		std::vector<std::unique_ptr<hhl::thread>> threads_;
		std::deque<Task>		queue_;
		size_t					maxQueueSize_;
		bool					running_;
	};
}



#endif // !THREADPOOL_H
