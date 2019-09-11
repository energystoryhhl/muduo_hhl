#include "TimeStamp.h"

#include <sys/time.h>

namespace hhl
{
namespace base
{
     TimeStamp TimeStamp::now()
    {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        return TimeStamp(tv.tv_sec * TimeStamp::KMicronSecondsPerSecond + tv.tv_usec);
    }

    std::string TimeStamp::toString()
    {
        char buf[32] = {0};
        int64_t second = microSecondSinceEpoch_ / KMicronSecondsPerSecond;
        int64_t micronSecond = microSecondSinceEpoch_ & KMicronSecondsPerSecond;
        snprintf(buf,sizeof(buf),"%ld.%06ld",second,micronSecond);
        return buf;
    }

    std::string TimeStamp::toFormatString()
    {
        char buf[64] = {0};
        struct tm time_tm;
        time_t second = static_cast<time_t>(microSecondSinceEpoch_ / KMicronSecondsPerSecond);
        gmtime_r(&second,&time_tm);

        int64_t miconSecond = static_cast<int64_t>(microSecondSinceEpoch_ % KMicronSecondsPerSecond);

        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06ld",
        time_tm.tm_year + 1900, time_tm.tm_mon + 1, time_tm.tm_mday,
        time_tm.tm_hour + 8, time_tm.tm_min, time_tm.tm_sec, miconSecond
        );
        return buf;
    }


}

}