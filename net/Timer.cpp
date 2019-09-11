#include "Timer.h"
#include "TimeStamp.h"

using namespace hhl::base;

namespace hhl
{
	namespace net 
	{
		void Timer::restart(base::TimeStamp now)
		{
			if (repeat_)
			{
				expiration_ = addTime(now, interval_);
			}
			else {
				expiration_ = TimeStamp();
			}


		}

	}//namespace net 
}