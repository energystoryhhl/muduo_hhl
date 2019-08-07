#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include "noncopyable.h"
#include "../base/types.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>

namespace hhl {

namespace CurrentThread {

	extern __thread int t_cachedTid;
	extern __thread char t_tidString[32];
	extern __thread int t_tidStringLength;
	extern __thread const char* t_threadName;

	void cacheTid();

	inline int tid()
	{
		if (t_cachedTid == 0)
		{
			cacheTid();
		}
		return t_cachedTid;
	}

}// ns CurrentThread
}// ns hhl


#endif // !CURRENTTHREAD_H
