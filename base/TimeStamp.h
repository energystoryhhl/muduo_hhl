#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <functional>

#include "time.h"

namespace hhl
{
namespace base
{

    class TimeStamp 
    {
    private:
        /* data */
    int64_t microSecondSinceEpoch_;

    public:
        TimeStamp(/* args */)
        :
        microSecondSinceEpoch_(0)
        {

        }

		bool valid() const { return microSecondSinceEpoch_ > 0; }

        explicit TimeStamp(int64_t t)
        :
        microSecondSinceEpoch_(t)
        {
			
        }

        void swap(TimeStamp & t)
        {
            std::swap(this->microSecondSinceEpoch_, t.microSecondSinceEpoch_);
        }

        bool vaild(){return microSecondSinceEpoch_>0? 1:0; }

        //~TimeStamp();

        int64_t microSecondSinceEpoch()const{ return microSecondSinceEpoch_;}

        time_t SecondSinceEpoch(){ return static_cast<time_t>(microSecondSinceEpoch_/KMicronSecondsPerSecond); }

		static TimeStamp now();

        std::string toString();

        std::string toFormatString();

       static const int64_t KMicronSecondsPerSecond = 1000 * 1000;
    };
    
    inline bool operator == (TimeStamp l, TimeStamp r)
    {
        return l.microSecondSinceEpoch() == r.microSecondSinceEpoch();
    } 

    inline bool operator<( TimeStamp  l,  TimeStamp  r)
    {
        return l.microSecondSinceEpoch() < r.microSecondSinceEpoch();
    }

	inline TimeStamp addTime(TimeStamp timestamp, double seconds)
	{
		int64_t delta = static_cast<int64_t>(seconds * TimeStamp::KMicronSecondsPerSecond);
		return TimeStamp(timestamp.microSecondSinceEpoch() + delta);
	}
}//namespace base
}//namespace hhl

#endif // !TIMESTAMP_H