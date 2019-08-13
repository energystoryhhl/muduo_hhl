#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "noncopyable.h"
#include "mutex.h"
#include "Condition.h"
#include "iostream"

namespace hhl{

class CountDownLatch : noncopyable
{
    private:
    mutable hhl::MutexLock mutex_;
    int count_;
    hhl::Condition cond_;

    public:
    CountDownLatch(int count)
    :
    mutex_(),
    count_(count),
    cond_(mutex_)
    {

    }

    ~CountDownLatch()
    {

    }

    void wait();

    void countDown();
    
    int getCount() const;


};

}//namespace hhl



#endif // !COUNTDOWNLATCH_H
