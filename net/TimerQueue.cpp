#include "TimerQueue.h"
#include "sys/timerfd.h"
#include "Logging.h"

namespace hhl
{
	namespace net
	{
		int createTimerfd()
		{
			int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
				TFD_NONBLOCK | TFD_CLOEXEC);
			if (timerfd < 0)
			{
				LOG_DEBUG << "Failed in timerfd_create";
			}
			return timerfd;
		}

		TimerQueue::TimerQueue(EventLoop * loop)
			:
			loop_(loop),
			timerfd_(createTimerfd()),
			timerfdChannel_(loop_, timerfd_),
			timers_(),
			callingExpiredTimers(false)
		{
			timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
			timerfdChannel_.enableReading();
		}

		TimerQueue::~TimerQueue()
		{
		}

		TimerId TimerQueue::addTimer(TimerCallback cb, base::TimeStamp when, double interval)
		{
			Timer* timer = new Timer(std::move(cb), when, interval);

			loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
			return TimerId(timer, timer->sequence());
		}

		void TimerQueue::cancel(TimerId *timerId)
		{
			loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
		}

		void TimerQueue::addTimerInLoop(Timer * timer)
		{
		}

		void TimerQueue::cancelInLoop(TimerId * timerId)
		{
		}

		void TimerQueue::handleRead()
		{
		}
		

		std::vector<TimerQueue::Entry> TimerQueue::getExpired(base::TimeStamp now)
		{
			return std::vector<TimerQueue::Entry>();
		}

		void TimerQueue::reset(const std::vector<Entry>& expired, base::TimeStamp now)
		{
		}

		bool TimerQueue::insert(Timer * timer)
		{
			return false;
		}





	}
}