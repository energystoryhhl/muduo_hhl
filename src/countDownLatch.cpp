#include "countDownLatch.h"



namespace hhl{

    void CountDownLatch::wait()
    {
        hhl::MutexLockGuard lock(mutex_);
        //std::cout<<"countdownlatch wait lock!"<<std::endl;
        while ((count_ > 0))
        {
            std::cout << "in wait!\n" << std::endl;
            cond_.wait(); 
        } 
        //std::cout<<"countdownlatch wait unlock!"<<std::endl;
    }

    void CountDownLatch::countDown()
    {
        hhl::MutexLockGuard lock(mutex_);
        //std::cout<<"countdownlatch countdown lock!"<<std::endl;
        count_ --;
        if(count_ == 0)
        {
            cond_.notifyAll();
        }
         //std::cout<<"countdownlatch countdown unlock!"<<std::endl;
    }

    int CountDownLatch::getCount() const
    {
        hhl::MutexLockGuard lock(mutex_);
        return count_;
    }



}//namespace hhl{