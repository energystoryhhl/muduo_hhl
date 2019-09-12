#include "EventLoop.h"

#include "CurrentThread.h"
#include "poller.h"
#include "Channel.h"
#include "TimerQueue.h"

namespace hhl
{
	namespace net
	{
		EventLoop::EventLoop()
			:
			threadId_(CurrentThread::tid())
		{

		}

		EventLoop::~EventLoop()
		{
		}

	}
}