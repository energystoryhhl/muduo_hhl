#ifndef LOGFILE_H
#define LOGFILE_H

#include "time.h"
#include <string>

using std::string;

namespace hhl
{
	namespace FileUtil
	{

		class LogFile
		{
		private:
			/* data */
			time_t startTIme_;
			time_t lastRoll_;
			time_t lastFlush_;

			const int flushInterval_;
			const off_t rollSize_;
			const int checkEveryN_;

			int count_;


		public:
			LogFile(const string& basename,
				off_t rollSize,
				bool threadSafe = true,
				int flushInterval = 3,
				int checkEveryN = 1024);

			void roll();

			LogFile();
		};


	}

}

#endif //LOGFILE_H