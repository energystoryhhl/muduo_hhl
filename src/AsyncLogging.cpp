#include "AsyncLogging.h"

#define ASYNCDEBUG 1
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
                    cond_(mutex_),
                    running_(false),
                    currentBuffer_(new Buffer),
                    nextBuffer_(new Buffer),
                    buffers_(),//will to be write
                    thread_(std::bind(&hhl::AsyncLogging::threadFunc,this),"threadFunc")
                    {
                        currentBuffer_->bzero();
                        nextBuffer_->bzero();
                        buffers_.reserve(16);
                    }
            AsyncLogging::~AsyncLogging()
            {
                if(running_)
                {
                    stop();
                }
            }

    //append ֻ����buffer����д
    void AsyncLogging::append(const char *logline, size_t len) 
    {
#ifdef ASYNCDEBUG
if(len != 0)
    std::cout<<"async append! size: "<<len<<std::endl;
#endif // DEBUG
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
            cond_.notify();
            //currentBuffer_ = std::move(nextBuffer_); //��curָ��next��next����Ϊnull
        }

    }

    void AsyncLogging::threadFunc()
    {
        std::cout<<"start async thread!"<<std::endl;
        assert(running_ == true);
        //latch
        FileUtil::LogFile output(basename_, rollSize_, false);
        BufferPtr newBuffer1(new Buffer);
        BufferPtr newBuffer2(new Buffer);

        BufferVector bufferToWrite;
        bufferToWrite.reserve(16);

        while (running_)
        {
            /* code */
#ifdef ASYNCDEBUG
 //std::cout<<"L";
#endif // DEBUG

            assert(newBuffer1 &&  newBuffer1->length() == 0);
            assert(newBuffer2 &&  newBuffer2->length() == 0);
            assert(bufferToWrite.empty());

            {
                hhl::MutexLockGuard lock(mutex_);
#ifdef ASYNCDEBUG
if(buffers_.size()!=0)
    std::cout<<"buffers size: "<<buffers_.size()<<std::endl;
#endif // DEBUG
                if(buffers_.empty())
                {
                    cond_.waitForSeconds(2);
                }
                buffers_.push_back(std::move(currentBuffer_));
                currentBuffer_ = std::move(newBuffer1);
                bufferToWrite.swap(buffers_);
                if(!nextBuffer_)
                {
                    nextBuffer_ = std::move(newBuffer2);
                }
            }

            assert(!bufferToWrite.empty());

            if(bufferToWrite.size() > 25)
            {
                output.append("drop log!",20);
                bufferToWrite.erase(bufferToWrite.begin() + 2, bufferToWrite.end());
            }
 
            for(const auto & buffer : bufferToWrite)
            {
                if(buffer->length() != 0)
                {
                    std::cout<<"buffer size: "<<buffer->length()<<std::endl;
                }
                output.append(buffer->data(), buffer->length());
            }

            if(bufferToWrite.size() > 2)
            {
                bufferToWrite.resize(2);
            }

            if(newBuffer1 == NULL)
            {
                newBuffer1 = std::move(bufferToWrite.back());
                newBuffer1->reset();
                bufferToWrite.pop_back();
            }

            if(newBuffer2 == NULL)
            {
                newBuffer2 = std::move(bufferToWrite.back());
                newBuffer2->reset();
                bufferToWrite.pop_back();
            }

            bufferToWrite.clear();
            output.flush();
        }
        output.flush();
    }



}//namespace hhl

