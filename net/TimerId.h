#ifndef TIMERID_H
#define TIMERID_H

#include "Timer.h"

namespace hhl
{
	namespace net
	{
		class Timer;

		class TimerId
		{
		public:
			TimerId()
				:
				timer_(NULL),
				sequence_(0)
			{

			}

			TimerId(Timer * timer, int64_t seq)
				:
				timer_(timer),
				sequence_(seq)
			{

			}
			friend class TimerQueue;
		private:
			Timer * timer_;
			int64_t sequence_;


		};

	}//namespace net
}



#endif // !TIMERID_H

