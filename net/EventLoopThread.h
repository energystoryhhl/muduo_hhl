#pragma once

#include "thread.h"
#include "mutex.h"
#include "Condition.h"

namespace hhl
{
	namespace net
	{
		class EventLoop;

		class EventLoopThread : noncopyable
		{
		public:
			typedef std::function<void(EventLoop*)> ThreadInitCallback;

			EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
				const std::string& name = std::string());

			~EventLoopThread();

			EventLoop* startLoop();

		private:
			void threadFunc();

			EventLoop*		loop_;
			bool			exiting_;
			thread			thread_;
			MutexLock		mutex_;
			Condition		cond_;
			ThreadInitCallback	callback_;


		};




	}
}