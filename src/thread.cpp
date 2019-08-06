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

    void * startFunc(void *p)
    {
        thread * pthis = static_cast<thread *>(p);
        pthis->latch_.countDown();

        pthis->func_();

        return NULL;
    }


    void thread::start()
    {
        assert(!started_);
        started_ = true;
        latch_.countDown();

        if(pthread_create(&pthreadId_, NULL, &startFunc, this))
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