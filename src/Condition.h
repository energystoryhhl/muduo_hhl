#ifndef CONDITION_H
#define CONDITION_H

#include "noncopyable.h"
#include "mutex.h"

namespace hhl{

class Condition : noncopyable
{
private:
MutexLock mutex_;
pthread_cond_t pcond_;

public:
    Condition(MutexLock& mutex)
    :
    mutex_(mutex)
    {
        pthread_cond_init(&pcond_,NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }
};

}//namespace hhl{

#endif //#ifndef CONDITION_H