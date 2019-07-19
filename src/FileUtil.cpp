#include "FileUtil.h"




namespace hhl
{

	namespace FileUtil {
		char t_errnobuf[1024];
		const char* strerror_tl(int savedErrno)
		{
			return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
		}
	}

	FileUtil::AppendFile::AppendFile(std::string fileName)
		:fp_(::fopen(fileName.c_str(), "a+")),
		writtenBytes_(0)
	{
		assert(fp_);
		::setbuffer(fp_, buffer_, sizeof(buffer_));
	}

	void FileUtil::AppendFile::append(const char *msg, size_t len)
	{
		size_t n = write(msg, len);
		size_t remain = n - len;
		while (remain > 0)
		{
			size_t x = write(msg, remain);
			if (0 == x)
			{
				int err = ferror(fp_);
				if (err)
				{
					fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
				}
				break;
			}
			remain = remain - x;
		}
		writtenBytes_ += len;

	}

	void FileUtil::AppendFile::append(const char *msg)
	{
		append(msg, strlen(msg));
	}

	void FileUtil::AppendFile::flush()
	{
		fflush(fp_);
	}

	size_t FileUtil::AppendFile::WrittenBytes() const
	{
		return writtenBytes_;
	}

	void FileUtil::AppendFile::close()
	{
		fclose(fp_);
	}


	size_t FileUtil::AppendFile::write(const char * msg, size_t len)
	{
		return fwrite_unlocked(msg, 1, len, fp_);
	}



}//namespace hhl