#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "noncopyable.h"
#include "fcntl.h"
#include "stdio.h"
#include <string>
#include "assert.h"
#include "sys/stat.h"
#include "stdio.h"
#include "err.h"
#include "errno.h"
#include "string.h"

namespace hhl
{

	namespace FileUtil
	{

		extern char t_errnobuf[1024];

		class AppendFile : noncopyable
		{
		public:
			AppendFile(std::string fileName);

			void append(const char *msg, size_t len);

			void append(const char *msg);

			void flush();

			void close();

			size_t WrittenBytes() const;


		private:
			size_t write(const char * msg, size_t len);
			FILE* fp_;
			char buffer_[60 * 1024];
			unsigned long writtenBytes_;

		};

	}//namespace FileUtil 
} // namespace hhl





#endif //FILEUTIL_H