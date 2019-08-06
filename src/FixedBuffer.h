#ifndef FIXEDBUFFER_H
#define FIXEDBUFFER_H

#include "noncopyable.h"
#include "string.h"
namespace hhl
{
    const int KSmallBuffer = 4000;
    const int KLargeBuffer = 4000 * 1000;

    template<int SIZE>
    class FixedBuffer : noncopyable
    {
    private:
        /* data */
        char * cur_;
        char  data_[SIZE];
        
    public:
        FixedBuffer(/* args */)
        :cur_(data_)
        {

        }
        
        //~FixedBuffer();

        void append(const char * buf, size_t len)
        {
            if(static_cast<int>(avail()) > len)
            {
                memcpy(cur_, buf, len);
                cur_ += len;
            }
        }

        int length() const{return static_cast<int>(cur_ - data_);}

        const char* data() const {return data_;}

        char *current() const {return cur_;}

        void bzero() const {memset((void *)(data_), 0, sizeof(data_));}

        const char * end() const{return data_ + sizeof(data_);}

        int avail() const {return static_cast<int>(end() - cur_); }

        void reset(){cur_ = data_;}
    };

     
} // namespace hhl



#endif

