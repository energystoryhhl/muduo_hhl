#include "LogFile.h"

namespace hhl
{
	namespace FileUtil
	{

		LogFile::LogFile(const string& basename,
			off_t rollSize,
			bool threadSafe,
			int flushInterval,
			int checkEveryN)
			: flushInterval_(flushInterval),
			rollSize_(rollSize),
			checkEveryN_(checkEveryN)
		{


		}

		void FileUtil::LogFile::roll()
		{

		}

	}//namespace FileUtil

}