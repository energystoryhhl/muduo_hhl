#include "thread.h"

namespace hhl
{
    thread::thread(Threadfunc func, const std::string &name )
    :
    started_(false),
    joined_(false),
    pthreadId_(0),
    name_(name),
    func_(std::move(func)),
    tid_(0),
    latch_(1)
    {

    }

    void thread::start()
    {
        assert(!started_);
        started_ = true;
        if(pthread_create(&pthreadId_,NULL,*func_.target<void*(*)(void *)>() ,NULL))
        {
            started_ = false;
            perror("thread start failed!\n");
        }else{
            latch_.wait();
            //assert
        }
        tid_ = CurrentThread::tid();
    }

    void thread::join()
    {
        assert(started_);
        assert(!joined_);
        joined_ = true;
        pthread_join(pthreadId_,NULL);
    }

    thread::~thread()
    {
        if(started_ && joined_)
        {
            pthread_detach(pthreadId_);
        }
    }

}