#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "mutex.h"
#include "FixedBuffer.h"

#include <string>
#include <vector>
#include <atomic>
#include <memory>
using namespace std;

namespace hhl
{
    class AsyncLogging
    {
    private:
    
        void threadFunc();
        /* data */
        const string & basename_;
        off_t rollSize_;
        int flushInterval_;

        hhl::MutexLock mutex_;

        typedef hhl::FixedBuffer<hhl::KLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        
        BufferPtr currentBuffer_;/*std::unique_ptr<Buffer>*/
        BufferPtr nextBuffer_;
        BufferVector buffers_;

    public:
        /* */
        AsyncLogging(const string & basename,
                    off_t rollSize,
                    int flushInterval = 3);

        ~AsyncLogging();

        /*fun*/
        void append(const char *logline, size_t len);
    };
    




    
}//namespace hhl


#endif //ASYNCLOGGING_H