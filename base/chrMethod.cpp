#include "chrMethod.h"


// root/test.bin -> test.bin
const char * CouperChr(const char * arr, char c)
{
    const char * slash = strrchr(arr,c);
    if(slash)
    {
        slash = slash + 1;
        return slash;
    }
    return NULL;
}