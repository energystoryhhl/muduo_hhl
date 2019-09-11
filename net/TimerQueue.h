#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include "Timer.h"
#include "TimerId.h"
#include "noncopyable.h"
#include "EventLoop.h"
#include "Channel.h"

#include <set>
#include <vector>

namespace hhl
{
	namespace net
	{

		class TimerQueue : noncopyable
		{
		public:
			explicit TimerQueue(EventLoop* loop);
			typedef std::pair<base::TimeStamp, Timer> Entry;

			~TimerQueue();

			TimerId addTimer(TimerCallback cb,
				base::TimeStamp when,
				double interval);

			void cancel(TimerId timerId);
		private:
			
			typedef std::set<Entry> TimerList;
			typedef std::pair<Timer*, int64_t> ActiveTimer;
			typedef std::set<ActiveTimer> ActiveTimerSet;

			void addTimerInLoop(Timer * timer);

			void cancelInLoop(TimerId* timerId);

			void handleRead();

			std::vector<Entry> getExpired(base::TimeStamp now);

			void reset(const std::vector<Entry>& expired, base::TimeStamp now);

			bool insert(Timer* timer);

			EventLoop* loop_;

			const int timerfd_;

			Channel timerfdChannel_;

			TimerList timers_;

			ActiveTimerSet activeTimers_;
			bool callingExpiredTimers;
			ActiveTimerSet cancelingTimers_;
		};
	}
}

#endif // !TIMERQUEUE_H



