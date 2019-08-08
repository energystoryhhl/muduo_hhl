#include "AsyncLogging.h"


namespace hhl
{
            AsyncLogging::AsyncLogging(const string & basename,
                    off_t rollSize,
                    unsigned long bufferSize ,
                    int flushInterval 
                    )
                    :
                    basename_(basename),
                    rollSize_(rollSize),
                    bufferSize_(bufferSize),
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

    void AsyncLogging::append(char *logline,size_t len)
    {
        
    }

}//namespace hhl

