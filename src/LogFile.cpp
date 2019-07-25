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
			: 
			basename_(basename),			//log基础文件名
			flushInterval_(flushInterval),	//文件flush间隔
			rollSize_(rollSize),			//文件写入字节rollsize
			checkEveryN_(checkEveryN),		//检测 rollfile步长
			lastRoll_(0),					//
			lastFlush_(0),
			startTime_(0),
			count_(0)
		{
			assert(basename.find('/') == string::npos);
			rollFile();
		}

		bool FileUtil::LogFile::rollFile()
		{
			time_t now = 0;
			string filename = getLogFileName(basename_, &now);

			time_t start = now / (60 * 60 * 24) * (60 * 60 * 24);

			if (now > lastRoll_)
			{
				lastRoll_ = now;
				lastFlush_ = now;
				startTime_ = start;
				file_.reset(new FileUtil::AppendFile(filename));

				return true;
			}
			return false;

		}

		string LogFile::getLogFileName(const string & basename, time_t * now)
		{
			string filename;
			filename.reserve(basename.size() + 64);
			filename = basename;

			char timebuf[32];
			struct tm tm;
			*now = time(NULL);
			gmtime_r(now, &tm);
			strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
			filename += timebuf;

			filename += ProcessInfo::hostname();

			char pidbuf[32] = { 0 };
			sprintf(pidbuf, ".%d", ProcessInfo::pid());
			filename += pidbuf;

			filename += ".log";

			return filename;
		}

		void LogFile::append_unlocked(const char * logline, int len)
		{
			file_->append(logline, len);

			if (file_->WrittenBytes() > rollSize_)
			{
				rollFile();
			}
			else
			{
				++count_;
				if (count_ >= checkEveryN_)
				{
					count_ = 0;
					time_t now = ::time(NULL);
					time_t thisPeriod_ = now / (60 * 60 * 24) * (60 * 60 * 24);
					if (thisPeriod_ != startTime_)
					{
						rollFile();
					}
					else if (now - lastFlush_ > flushInterval_)
					{
						lastFlush_ = now;
						file_->flush();
					}
				}
			}
		}

	}//namespace FileUtil

}