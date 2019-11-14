#pragma once

#include "noncopyable.h"
#include "base/types.h"

#include <functional>
#include <memory>
#include <vector>

namespace hhl
{
	namespace net
	{

		class EventLoop;
		class EventLoopThread;

		class EventLoopThreadPool :noncopyable
		{
		public:
			typedef std::function<void(EventLoop*)> ThreadInitCallback;

			EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);


			void setThreadNum(int numThreads) { numThreads_ = numThreads; }
			void start(const ThreadInitCallback& cb = ThreadInitCallback());

			EventLoop* getNextLoop();

		private:
			EventLoop* baseLoop_;
			std::string name_;
			bool started_;
			int numThreads_;
			int next_;
			std::vector<std::unique_ptr<EventLoopThread>> threads_;
			std::vector<EventLoop*> loops_;

		};

	}
}