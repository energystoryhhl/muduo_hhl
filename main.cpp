#include <iostream>
#include "Logging.h"


int main()
{
    std::cout<<"hello"<<std::endl;
    LOG_DEBUG<<"123"<<"this is a test log"<<"2333";
    return 0;
}