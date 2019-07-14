#include "FileUtil.h"


namespace hhl
{

FileUtil::AppendFile::AppendFile(std::string fileName)
:fp_(::fopen(fileName.c_str(),"w+")),
writtenBytes_(0)
{
    assert(fp_ );
    ::setbuffer(fp_,buffer_,sizeof(buffer_));
}

}//namespace hhl