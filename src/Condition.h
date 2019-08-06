#ifndef CONDITION_H
#define CONDITION_H

#include "noncopyable.h"
#include "mutex.h"
//#include <pthread.h>

using namespace hhl;

namespace hhl{

class Condition : noncopyable
{
private:
MutexLock &mutex_;
pthread_cond_t pcond_;

public:
    explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
    {
        pthread_cond_init(&pcond_,NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);//在mutex中申请了友元
        ::pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
       pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

    int waitForSeconds(int s);
};

}//namespace hhl{

#endif //#ifndef CONDITION_H