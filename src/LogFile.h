#ifndef LOGFILE_H
#define LOGFILE_H

#include "time.h"
#include "FileUtil.h"
#include <unistd.h>
#include <string.h>
#include <string>
#include <memory>

#include "Processinfo.h"
#include "mutex.h"

using std::string;

namespace hhl
{
	namespace FileUtil
	{

		class LogFile
		{
		private:
			/* data */


			const string basename_;
			const int flushInterval_;
			const off_t rollSize_;
			const int checkEveryN_;

			time_t lastRoll_;
			time_t lastFlush_;
			time_t startTime_;

			int count_;

			bool threadSafe_;


			std::unique_ptr<hhl::MutexLock> mutex_ ;

			static string getLogFileName(const string& basename, time_t* now);

			void append_unlocked(const char* logline, int len);

			std::unique_ptr<FileUtil::AppendFile> file_;
			//~LogFile();
			

		public:
			LogFile(const string& basename,
				off_t rollSize,
				bool threadSafe = true,
				int flushInterval = 3,
				int checkEveryN = 1024);

			bool rollFile();

			void append(const char* logline, int len);

			void flush();

		};


	}

}

#endif //LOGFILE_H