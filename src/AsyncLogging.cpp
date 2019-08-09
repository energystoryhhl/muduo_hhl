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

    //append ֻ����buffer����д
    void AsyncLogging::append(const char *logline, size_t len) 
    {
        hhl::MutexLockGuard lock(mutex_);
        if(currentBuffer_->avail() > len)
        {
            currentBuffer_->append(logline,len);
        }
        else
        {
            buffers_.push_back(std::move(currentBuffer_));//������buffer����buffer vector��
            if(nextBuffer_ != NULL)
            {
                currentBuffer_ = std::move(nextBuffer_); 
            }
            else
            {
                currentBuffer_.reset(new Buffer);
            }

            currentBuffer_->append(logline, len);
            
            //currentBuffer_ = std::move(nextBuffer_); //��curָ��next��next����Ϊnull
        }
        

    }




}//namespace hhl

