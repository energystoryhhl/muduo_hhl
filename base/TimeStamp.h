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

        explicit TimeStamp(int64_t t)
        :
        microSecondSinceEpoch_(t)
        {

        }

        void swap(TimeStamp & t)
        {
            std::swap(this->microSecondSinceEpoch_, t.microSecondSinceEpoch_);
        }

        bool vaild( return microSecondSinceEpoch_>0? 1:0;)

        ~TimeStamp();

        int64_t microSecondSinceEpoch(){ return microSecondSinceEpoch_;}

        time_t SecondSinceEpoch(){ return static_cast<time_t>(microSecondSinceEpoch_/KMicronSecondsPerSecond;) 
        
        }

        const int64_t KMicronSecondsPerSecond = 1000 * 1000;
    };
    
    


}//namespace base
}//namespace hhl

#endif // !TIMESTAMP_H