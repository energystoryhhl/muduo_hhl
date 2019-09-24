#include "TimerQueue.h"
#include "sys/timerfd.h"
#include "Logging.h"

namespace hhl
{
	namespace net
	{
		//void resetTimerfd(int timerfd, base::TimeStamp expiration)
		//{
		//	// wake up loop by timerfd_settime()
		//	struct itimerspec newValue;
		//	struct itimerspec oldValue;
		//	memZero(&newValue, sizeof newValue);
		//	memZero(&oldValue, sizeof oldValue);
		//	newValue.it_value = howMuchTimeFromNow(expiration);
		//	int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
		//	if (ret)
		//	{
		//		LOG_DEBUG << "timerfd_settime()";
		//	}
		//}

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
			loop_->assertInLoopThread();
			bool earliestChanged = insert(timer);


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
			base::TimeStamp nextExpire;
			for (const Entry& it : expired)
			{
				ActiveTimer timer(it.second, it.second->sequence());
				if (it.second->repeat()
					&& cancelingTimers_.find(timer) == cancelingTimers_.end())
				{
					it.second->restart(now);
					insert(it.second);
				}
				else
				{
					delete it.second; // FIXME: no delete please
				}

				if (!timers_.empty())
				{
					nextExpire = timers_.begin()->second->expiration();
				}

				if (nextExpire.valid())
				{
					resetTimerfd(timerfd_, nextExpire);
				}

			}


		}

		bool TimerQueue::insert(Timer * timer)
		{
			loop_->assertInLoopThread();
			assert(timers_.size() == activeTimers_.size());
			bool earliestChanged = false;

			 base::TimeStamp when = timer->expiration();
			TimerList::iterator it = timers_.begin();
			if (it == timers_.end() || when < const_cast<base::TimeStamp &>(it->first) )
			{
				earliestChanged = true;
			}

			{
				std::pair<TimerList::iterator, bool> result
					= timers_.insert(Entry(when, timer));

				assert(result.second); (void)result;
			}
			{
				std::pair<ActiveTimerSet::iterator, bool> result
					= activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
				assert(result.second); (void)result;
			}
			assert(timers_.size() == activeTimers_.size());
			return earliestChanged;
		}





	}
}