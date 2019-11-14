#include "EventLoopThread.h"
#include "net/EventLoop.h"

using namespace hhl;
using namespace hhl::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback & cb, const std::string & name)
	:loop_(NULL),
	exiting_(false),
	thread_(std::bind(&EventLoopThread::threadFunc, this), name),
	mutex_(),
	cond_(mutex_),
	callback_(cb)
{



}

EventLoop * hhl::net::EventLoopThread::startLoop()
{
	assert(!thread_.started());
	thread_.start();

	EventLoop* loop = NULL;
	{
		MutexLockGuard lock(mutex_);
		while (loop_ == NULL)
		{
			cond_.wait();
		}
		loop = loop_;
	}
	return loop;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;

	if (callback_)
	{
		callback_(&loop);
	}

	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}

	loop.loop();
	//assert(exiting_);
	MutexLockGuard lock(mutex_);
	loop_ = NULL;
}

EventLoopThread::~EventLoopThread()
{
	exiting_ = true;
	if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
	{
		// still a tiny chance to call destructed object, if threadFunc exits just now.
		// but when EventLoopThread destructs, usually programming is exiting anyway.
		loop_->quit();
		thread_.join();
	}
}
