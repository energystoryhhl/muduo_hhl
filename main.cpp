#include <iostream>
#include "Logging.h"
#include "FileUtil.h"

int main()
{
    std::cout<<"hello"<<std::endl;
    //LOG_DEBUG<<"123"<<"this is a test log"<<"2333";
    //string str = "logfile.log";
    hhl::FileUtil::AppendFile("log.log");

    return 0;
}