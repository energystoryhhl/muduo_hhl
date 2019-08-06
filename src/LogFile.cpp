#include "LogFile.h"
#include "mutex.h"
#include <iostream>


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
			basename_(basename),			//log�����ļ���
			flushInterval_(flushInterval),	//�ļ�flush���
			rollSize_(rollSize),			//�ļ�д���ֽ�rollsize
			checkEveryN_(checkEveryN),		//��� rollfile����
			lastRoll_(0),					//
			lastFlush_(0),
			startTime_(0),
			count_(0),
		threadSafe_(threadSafe ? new hhl::MutexLock : NULL)
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

		void LogFile::append(const char* logline, int len)
		{
			if (NULL == mutex_)
			{
				//std::cout<<logline<<"NULL"<<std::endl;
				append_unlocked(logline, len);
			}
			else
			{
				//TODO threadSAFE
				hhl::MutexLockGuard lock(*mutex_);
				//std::cout<<logline<<std::endl;
				append_unlocked(logline, len);
			}
		}

		void LogFile::flush()
		{
			if (NULL == mutex_)
			{
				file_->flush();
			}
			else
			{
				//TODO threadSAFE
				hhl::MutexLockGuard lock(*mutex_);
				file_->flush();
			}
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
			//file_->append("666", 5);
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