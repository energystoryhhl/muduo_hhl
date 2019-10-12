#include "TimerQueue.h"
#include "sys/timerfd.h"
#include "Logging.h"

namespace hhl
{
	namespace net
	{

		struct timespec howMuchTimeFromNow(base::TimeStamp when)
		{
			int64_t microseconds = when.microSecondSinceEpoch()
				- base::TimeStamp::now().microSecondSinceEpoch();
			if (microseconds < 100)
			{
				microseconds = 100;
			}
			struct timespec ts;
			ts.tv_sec = static_cast<time_t>(
				microseconds / base::TimeStamp::KMicronSecondsPerSecond);
			ts.tv_nsec = static_cast<long>(
				(microseconds % base::TimeStamp::KMicronSecondsPerSecond) * 1000);
			return ts;
		}

		void readTimerfd(int timerfd, base::TimeStamp now)
		{
			uint64_t howmany;
			ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
			LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
			if (n != sizeof howmany)
			{
				LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
			}
		}

		void resetTimerfd(int timerfd, base::TimeStamp expiration)
		{
			// wake up loop by timerfd_settime()
			struct itimerspec newValue;
			struct itimerspec oldValue;
			memset(&newValue, 0, sizeof(newValue));
			memset(&oldValue, 0, sizeof(oldValue));
			newValue.it_value = howMuchTimeFromNow(expiration);
			int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
			if (ret)
			{
				LOG_DEBUG << "timerfd_settime() ";
			}
		}

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
			/* 返回true，说明timer被添加到set的顶部，作为新的根节点，需要更新timerfd的激活时间 */
			bool earliestChanged = insert(timer);

			// 只有在计时器为空的时候或者新加入的计时器的最早触发时间小于当前计时器的堆顶的最小值
			// 才需要用最近时间去更新
			if (earliestChanged)
			{
				resetTimerfd(timerfd_, timer->expiration());
			}
		}

		void TimerQueue::cancelInLoop(TimerId * timerId)
		{
			loop_->assertInLoopThread();
			assert(timers_.size() == activeTimers_.size());
			ActiveTimer timer(timerId->timer_, timerId->sequence_);
			ActiveTimerSet::iterator it = activeTimers_.find(timer);
			if (it != activeTimers_.end())
			{
				size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
				assert(n == 1); (void)n;
				delete it->first; // FIXME: no delete please
				activeTimers_.erase(it);
			}
			else if (callingExpiredTimers)
			{
				cancelingTimers_.insert(timer);
			}
			assert(timers_.size() == activeTimers_.size());
		}

		void TimerQueue::handleRead()
		{
			loop_->assertInLoopThread();
			base::TimeStamp now(base::TimeStamp::now());
			//readtimerfd
			readTimerfd(timerfd_, now);

			std::vector<Entry> expried = getExpired(now);//获取过期时间

			callingExpiredTimers = true;
			cancelingTimers_.clear();
			for (const Entry& it : expried)
			{
				it.second->run();
			}
			callingExpiredTimers = false;

			reset(expried, now);
		}
		

		std::vector<TimerQueue::Entry> TimerQueue::getExpired(base::TimeStamp now)
		{
			assert(timers_.size() == activeTimers_.size());
			std::vector<Entry> expried;
			Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));

			TimerList::iterator end = timers_.lower_bound(sentry);
			assert(end == timers_.end() || now < end->first);
			std::copy(timers_.begin(), end, std::back_inserter(expried));
			timers_.erase(timers_.begin(), end);

			for (const Entry& it : expried)
			{
				ActiveTimer timer(it.second, it.second->sequence());
				size_t n = activeTimers_.erase(timer);
				assert(n == 1); (void)n;
			}

			assert(timers_.size() == activeTimers_.size());
			return expried;
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
					delete it.second; // FIXME: no delete please //重复
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
			if (it == timers_.end())
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