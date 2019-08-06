#ifndef THREAD_H
#define THREAD_H

#include "functional"
#include "thread.h"
#include "countDownLatch.h"

namespace hhl
{
    // void * startFunc(void *)
    // {
    //     thread * pthis = static_cast<thread *>(this);
    //     //pthis->latch_.countDown();
        
    //     //this->latch_.coutn
    // }

    class thread
    {
    public:
        typedef std::function<void()> Threadfunc;

    private:
        /* data */
        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        std::string name_;
        Threadfunc func_;
        pid_t tid_;
        CountDownLatch latch_;

        void setDefaultName()
        {
            if(name_.empty())
            {
                name_ = "thread";
            }
        }


    public:
        explicit thread(Threadfunc func, const std::string &name = std::string());

        ~thread();

        void start();

        void join();

        bool started() const {return started_;}

        bool joined() const {return joined_;}

        pid_t tid() const {return tid_;}

        const std::string & name() const {return name_;}

        friend void * hhl::startFunc(void *p);



    };
    
    
}

#endif // !THREAD_H