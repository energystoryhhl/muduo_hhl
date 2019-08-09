#include "AsyncLogging.h"


namespace hhl
{
            AsyncLogging::AsyncLogging(const string & basename,
                    off_t rollSize,
                    int flushInterval 
                    )
                    :
                    basename_(basename),
                    rollSize_(rollSize),
                    flushInterval_(flushInterval),
                    mutex_(),
                    currentBuffer_(new Buffer),
                    nextBuffer_(new Buffer),
                    buffers_()
                    {
                        currentBuffer_->bzero();
                        nextBuffer_->bzero();
                        buffers_.reserve(16);
                    }
            AsyncLogging::~AsyncLogging()
            {

            }

    //append 只是往buffer里面写
    void AsyncLogging::append(const char *logline, size_t len) 
    {
        hhl::MutexLockGuard lock(mutex_);
        if(currentBuffer_->avail() > len)
        {
            currentBuffer_->append(logline,len);
        }
        else
        {
            buffers_.push_back(std::move(currentBuffer_));//将满的buffer放入buffer vector中
            if(nextBuffer_ != NULL)
            {
                currentBuffer_ = std::move(nextBuffer_); 
            }
            else
            {
                currentBuffer_.reset(new Buffer);
            }

            currentBuffer_->append(logline, len);
            
            //currentBuffer_ = std::move(nextBuffer_); //将cur指向next，next设置为null
        }
        

    }




}//namespace hhl

