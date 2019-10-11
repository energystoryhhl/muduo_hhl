#ifndef EVENTLOOP_H
#define EVENTLOOP_H

//#include "noncopyable.h"
//#include "Atomic.h"
//#include "TimeStamp.h"
//#include "Epoller.h"
//#include "TimerQueue.h"
//#include "Channel.h"
//#include "poller.h"

#include <memory>
#include <atomic>
#include <functional>
#include <vector>

#include "mutex.h"
#include "TimeStamp.h"
#include "TimerId.h"

namespace hhl
{
namespace net
{
	class Channel;
	class Poller;
	class TimerQueue;

class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    //TODO
    EventLoop();
    ~EventLoop();

	void handleRead();

	void loop();

	void quit();

	void wakeup();

	void runInLoop(Functor cb);

	void queneInLoop(Functor cb);

	//channels
	void updateChannel(Channel* channel);

	void removeChannel(Channel* channel);

	bool hasChannel(Channel* channel);

	bool isRun() const;

	//timequeue
	size_t queueSize();

	TimerId runAt(base::TimeStamp time, TimerCallback cb);

	TimerId runEvery(double interval, TimerCallback cb);

	void abortNotInLoopThread();

	void assertInLoopThread()
	{
		if (!isInLoopThread())
		{
			abortNotInLoopThread();
		}
	}

	bool isInLoopThread() const {
		return threadId_ == CurrentThread::tid(); 
		//return true;
	}

	void printActiveChannels() const;



private:

	void doPendingFunctors();

	typedef std::vector<Channel*> ChannelList;
	
	bool looping_;
	std::atomic<bool> quit_;
	bool eventHandling_;
	bool callingPendingFunctors_;
	int64_t iteration_;
	const pid_t threadId_;
	base::TimeStamp pollReturnTime_;
	std::unique_ptr<Poller> poller_;
	std::unique_ptr<TimerQueue> timerQueue_;
	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;

	ChannelList activeChannels_;
	Channel* currentActiveChannel_;

	mutable MutexLock mutex_;
	std::vector<Functor> pendingFunctors_;
};	

} //namespace net
} //namespace hhl

#endif // !LOOPEVENT_H