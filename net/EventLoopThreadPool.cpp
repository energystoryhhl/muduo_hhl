#include "EventLoopThreadPool.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"


#include <assert.h>

using namespace hhl;
using namespace hhl::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg)
	: baseLoop_(baseLoop),
	name_(nameArg),
	started_(false),
	numThreads_(0),
	next_(0)
{




}

hhl::net::EventLoopThreadPool::~EventLoopThreadPool()
{




}

void hhl::net::EventLoopThreadPool::start(const ThreadInitCallback & cb)
{
	assert(!started_);
	baseLoop_->assertInLoopThread();

	started_ = true;
	for (int i = 0; i < numThreads_; ++i)
	{
		char buf[name_.size() + 32];
		snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
		EventLoopThread* t = new EventLoopThread(cb, buf);
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		loops_.push_back(t->startLoop());
	}
	if (numThreads_ == 0 && cb)
	{
		cb(baseLoop_);
	}
}

EventLoop * hhl::net::EventLoopThreadPool::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	assert(started_);

	EventLoop * loop = baseLoop_;

	if (!loops_.empty())
	{
		loop = loops_[next_];
		++next_;
		if (next_ >= loops_.size())
		{
			next_ = 0;
		}
	}
	return loop;
}





