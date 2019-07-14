#include "timeMethod.h"


timeStamp_t GetTimeStamp()
{
    struct timeval stamp;
    gettimeofday(&stamp,NULL);
    return stamp.tv_sec;
}




