#include "Logging.h"


namespace hhl{

void OutPutCout(const char * log,int len)
{
    std::cout<<log<<std::endl;
}

const char* Logger::LogLevelName[Logger::NUM_LOG_LEVELS]={
"TRACE","DEBUG","INFO","WARN","ERROR","FATAL"
};

Logger::LogLevel Logger::g_logLevel =Logger::DEBUG;

Logger::OutPutFunc Logger::p_outPutFUnc_ = OutPutCout;

void Logger::setOutPutFunc(OutPutFunc func)
{
    p_outPutFUnc_ = func;
}

std::ostringstream & Logger::stream()
{
    return logBuf_;
}


Logger::Logger(SourceFIle file,int line, LogLevel level, const char * func)
{
    logBuf_<<GetTimeStamp()<<"-"<<file.name()<<"-"<<LogLevelName[level]<<"-"<<func;
}

Logger::~Logger()
{
    logBuf_<<"-[end]"<<std::endl;
    p_outPutFUnc_(logBuf_.str().c_str(),static_cast<int>(logBuf_.str().length()));
}

}//namespace hhl
