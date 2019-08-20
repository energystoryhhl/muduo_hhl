#ifndef LOGGING_H
#define LOGGING_H
#include "chrMethod.h"
#include "string.h"
#include <string>
#include "timeMethod.h"
#include <sstream>
#include <iostream>

using std::string;



namespace hhl{

    class Logger{
        public:

        enum LogLevel{
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };


        /* /test/bin.c -> bin.c */
        class SourceFIle
        {
            public:
            template<int N>
            SourceFIle(const char (&arr)[N])
            :data_(arr),
            size_(N-1)
            {
                const char * slash = strchr(data_,'/');
                if(slash != NULL)
                {
                    data_ = slash + 1;
                    size_ = static_cast<int>(data_ - arr) ;
                }

            }

        
            explicit SourceFIle(const char * arr)
            :data_(arr)
            {
                const char * slash = (strrchr(arr, '/')) ;
                if(slash != NULL)
                {
                    data_ = slash + 1;
                    size_ = static_cast<int>(strlen(data_)) ;
                }

            }
            const char * name()
            {
                return data_;
            }
            private:
            const char *data_;
            int size_;
        };

        //Logger(SourceFIle file,int line, LogLevel level, const char * func);
Logger(SourceFIle file, int line, LogLevel level, const char * func);
        ~Logger();

        static LogLevel g_logLevel;

        /**/
        //

        static LogLevel logLevel()
        {
            return g_logLevel;
        }

        static void  SetLogLevel(LogLevel l)
        {
            g_logLevel = l;
        }

        static const char* LogLevelName[NUM_LOG_LEVELS];

        typedef void (* OutPutFunc)(const char* log,int len);

        static void setOutPutFunc(OutPutFunc func);

        static void OutPut(const char* log,int len)
        {
            if(p_outPutFUnc_ != NULL)
            {
                p_outPutFUnc_( log, len);
            }
        }

        std::ostringstream & stream();

#define LOG_DEBUG if(hhl::Logger::g_logLevel <= hhl::Logger::DEBUG) \
    hhl::Logger(__FILE__,__LINE__,hhl::Logger::DEBUG,__func__).stream()


#define LOG_TRACE if(hhl::Logger::g_logLevel <= hhl::Logger::TRACE) \
    hhl::Logger(__FILE__,__LINE__,hhl::Logger::DEBUG,__func__).stream()

		
#define LOG_ERROR if(hhl::Logger::g_logLevel <= hhl::Logger::ERROR) \
    hhl::Logger(__FILE__,__LINE__,hhl::Logger::DEBUG,__func__).stream()




    private:
        std::ostringstream logBuf_;
        static OutPutFunc  p_outPutFUnc_;

    };

}

#endif // ! 

