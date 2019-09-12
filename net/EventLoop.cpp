#include "EventLoop.h"

#include "CurrentThread.h"
#include "poller.h"
#include "Channel.h"
#include "TimerQueue.h"
#include "Logging.h"

#include <sys/eventfd.h>

namespace hhl
{
	namespace net
	{
		__thread EventLoop* t_loopInThisThread = 0;

		const int kPollTimeMs = 10000;

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
			quit_ = false;  // FIXME: what if someone calls quit() before loop() ?

			LOG_TRACE << " EventLoop " << this << " start looping ";

			while (quit_)
			{
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


			}



		}


		void EventLoop::printActiveChannels() const
		{
			for (const Channel* channel : activeChannels_)
			{
				LOG_TRACE << "{" << channel->reventsToString() << "} ";
			}
		}



	}
}