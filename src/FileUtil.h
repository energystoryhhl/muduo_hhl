#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "noncopyable.h"
#include "fcntl.h"
#include "stdio.h"
#include <string>
#include "assert.h"

namespace hhl
{

namespace FileUtil
{

class AppendFile : noncopyable
{
public:
    AppendFile(std::string fileName);

private:
    FILE* fp_;
    char buffer_[60 * 1024];
    unsigned long writtenBytes_;

};

}//namespace FileUtil 
} // namespace hhl





#endif //FILEUTIL_H