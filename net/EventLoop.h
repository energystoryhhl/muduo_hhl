#ifndef LOOPEVENT_H
#define LOOPEVENT_H

#include "src/noncopyable.h"
#include "Atomic.h"
#include "TimeStamp.h"
#include "Epoller.h"
#include "TimerQueue.h"
#include "Channel.h"
#include "poller.h"

#include <memory>
#include <atomic>
#include <functional>

namespace hhl
{
namespace net
{

class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    //TODO
    EventLoop();
    ~EventLoop();

private:

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
	
};

} //namespace net
} //namespace hhl



#endif // !LOOPEVENT_H