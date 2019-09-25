#ifndef TIMER_H
#define TIMER_H

#include "noncopyable.h"
#include "TimeStamp.h"
#include "Atomic.h"

#include <functional>

using namespace hhl::detail;

namespace hhl
{
	namespace net
	{
		
		static AtomicInt64 s_numCreated_;
		typedef std::function<void()> TimerCallback;

		class Timer : noncopyable
		{
		public:
			Timer(TimerCallback cb, base::TimeStamp when, double interval)
				:
				callback_(cb),
				expiration_(when),
				interval_(interval),
				repeat_(interval > 0.0),
				sequence_(s_numCreated_.incrementAndGet())//s_numCreated_.incrementAndGet()
			{

			}

			void run()
			{
				callback_();
			}

			base::TimeStamp expiration() const { return expiration_; }
			bool repeat() const { return repeat_; }
			int64_t sequence() const { return sequence_; }

			void restart(base::TimeStamp now);

			static int64_t numCreated() { return s_numCreated_.get(); }
			
			int64_t numIncrementAndGet() { return s_numCreated_.incrementAndGet(); }
		private:
			const TimerCallback callback_;
			base::TimeStamp expiration_;
			const double interval_;
			const bool repeat_;
			const int64_t sequence_;

		};
	}//namespace net
}

#endif // !TIMER_H
