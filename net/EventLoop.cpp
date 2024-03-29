#include "EventLoop.h"

#include "CurrentThread.h"
#include "poller.h"
#include "Channel.h"
#include "TimerQueue.h"
#include "Logging.h"

#include <algorithm>
#include <sys/eventfd.h>

namespace hhl
{
	namespace net
	{
		__thread EventLoop* t_loopInThisThread = 0;

		const int kPollTimeMs = 5000;

		int createEventfd()
		{
			int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
			if (evtfd < 0)
			{
				LOG_DEBUG << "Failed in eventfd";
				abort();
			}
			return evtfd;
		}


		EventLoop::EventLoop()
			:
			looping_(false),
			quit_(false),
			eventHandling_(false),
			callingPendingFunctors_(false),
			iteration_(0),
			threadId_(CurrentThread::tid()),
			poller_(Poller::newDefaultPoller(this)),
			timerQueue_(new TimerQueue(this)),
			wakeupFd_(createEventfd()),
			wakeupChannel_(new Channel(this, wakeupFd_)),
			currentActiveChannel_(NULL)
		{
			LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
			if (t_loopInThisThread)
			{
				LOG_DEBUG << "Another EventLoop " << t_loopInThisThread
					<< " exists in this thread " << threadId_;
			}
			else
			{
				t_loopInThisThread = this;
			}
			wakeupChannel_->setReadCallback(
				std::bind(&EventLoop::handleRead, this));
			// we are always reading the wakeupfd
			wakeupChannel_->enableReading();
		}

		EventLoop::~EventLoop()
		{
			LOG_DEBUG << "EventLoop " << this << " of thead " << threadId_ << " destructs in thread " << CurrentThread::tid();
			wakeupChannel_->disableAll();
			wakeupChannel_->remove();
			::close(wakeupFd_);
			t_loopInThisThread = NULL;
		}

		void EventLoop::handleRead()
		{
			uint64_t one = 1;
			ssize_t n = read(wakeupFd_, &one, sizeof(one));
			if (n != sizeof(one))
			{
				LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
			}
		}

		size_t EventLoop::queueSize()
		{
			MutexLockGuard lock(mutex_);
			return pendingFunctors_.size();
		}

		TimerId EventLoop::runAt(base::TimeStamp time, TimerCallback cb)
		{
			return timerQueue_->addTimer(std::move(cb), time, 0.0);
		}

		TimerId EventLoop::runEvery(double interval, TimerCallback cb)
		{
			base::TimeStamp time(base::addTime(base::TimeStamp::now(), interval));
			return timerQueue_->addTimer(std::move(cb), time, interval);
		}

		TimerId EventLoop::runAfter(double delay, TimerCallback cb)
		{
			base::TimeStamp time(base::addTime(base::TimeStamp::now(), delay));
			return runAt(time, cb);
		}

		void EventLoop::cancel(TimerId timerId)
		{
			return timerQueue_->cancel(&timerId);
		}

		void EventLoop::abortNotInLoopThread()
		{
			LOG_DEBUG << "EventLoop::abortNotInLoopThread - EventLoop " << this
				<< " was created in threadId_ = " << threadId_
				<< ", current thread id = " << CurrentThread::tid();
		}

		void EventLoop::loop()
		{
			assert(!looping_);
			assertInLoopThread();
			looping_ = true;
			quit_ = false;  

			LOG_TRACE << " EventLoop " << this << " start looping ";

			while (!quit_)
			{
				//LOG_DEBUG << "EventLoop " << this << "Running";
				activeChannels_.clear();
				pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_); //real poll in this
				++iteration_;

				if (Logger::logLevel() <= Logger::TRACE)
				{
					printActiveChannels();
				}

				eventHandling_ = true;
				for (Channel* channel : activeChannels_)
				{
					currentActiveChannel_ = channel;
					currentActiveChannel_->handleEvent(pollReturnTime_);
				}
				currentActiveChannel_ = NULL;
				eventHandling_ = false;
				
				doPendingFunctors();
			}

			LOG_DEBUG << "EventLoop " << this << " stop looping";
			looping_ = false;
			//quit_ = true;
		}

		void EventLoop::quit()
		{
			quit_ = true;

			if (!isInLoopThread())
			{
				wakeup();
			}

		}

		void EventLoop::wakeup()
		{
			uint64_t one = 1;
			ssize_t n = write(wakeupFd_, &one, sizeof(one));
			if (n != sizeof one)
			{
				LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
			}
		}

		void EventLoop::runInLoop(Functor cb)
		{
			if (isInLoopThread())
			{
				cb();
			}
			else {
				queneInLoop(cb);
			}
		}

		void EventLoop::queneInLoop(Functor cb)
		{
			{
				MutexLockGuard lock(mutex_);
				pendingFunctors_.push_back(std::move(cb));
			}

			if (!isInLoopThread() || callingPendingFunctors_)
			{
				wakeup();
			}

		}

		void EventLoop::updateChannel(Channel * channel)
		{
			assert(channel->ownerLoop() == this);
			assertInLoopThread();
			poller_->updateChannel(channel);
		}

		void EventLoop::removeChannel(Channel * channel)
		{
			assert(channel->ownerLoop() == this);
			assertInLoopThread();
			if (eventHandling_)
			{
				//TODO
				assert(std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end() || currentActiveChannel_ == channel);
			}
			poller_->removeChannel(channel);
		}

		bool EventLoop::hasChannel(Channel * channel)
		{
			assert(channel->ownerLoop() == this);
			assertInLoopThread();
			return poller_->hasChannel(channel);
		}

		bool EventLoop::isRun() const
		{
			return looping_;
		}


		void EventLoop::printActiveChannels() const
		{
			for (const Channel* channel : activeChannels_)
			{
				LOG_TRACE << "{" << channel->reventsToString() << "} ";
			}
		}

		void EventLoop::doPendingFunctors()
		{
			std::vector<Functor> functors;
			callingPendingFunctors_ = true;

			{
				MutexLockGuard lock(mutex_);
				functors.swap(pendingFunctors_);
			}

			for (const Functor& functor : functors)
			{
				functor();
			}
			callingPendingFunctors_ = false;
		}



	}
}