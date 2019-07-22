#include "Processinfo.h"

namespace hhl
{
	namespace ProcessInfo
	{
		string hostname()
		{
			char buf[256];
			if (::gethostname(buf, sizeof buf) == 0)
			{
				buf[sizeof(buf) - 1] = '\0';
				return buf;
			}
			else
			{
				return "unknownhost";
			}
		}
		pid_t pid()
		{
			return getpid();
		}
	}
}