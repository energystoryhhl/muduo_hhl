#include "Channel.h"

#include <memory>
#include <assert.h>
#include <sstream>
#include "sys/epoll.h"
#include <poll.h>

#include "Logging.h"

using namespace hhl;
using namespace hhl::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

namespace hhl
{
	namespace net 
	{
		//向epoll中添加fd
		void Channel::update()
		{
			addedToLoop_ = true;
			//loop_->updataChannel(this); //TODO
		}

		//向epoll中删除fd
		void Channel::remove()
		{
			assert(isNoneEvent());
			addedToLoop_ = false;
			//loop->removeChannel(this);

		}

		Channel::Channel(EventLoop * loop, int fd)
			:
		loop_(loop),
		fd_(fd),
		events_(0),revents_(0),index_(-1),
		logHup_(true),
		tied_(false),eventHandling_(false),addedToLoop_(false)
		{
		}

		Channel::~Channel()
		{
			assert(!eventHandling_);
			assert(!addedToLoop_);
			//TODO
		}

		void Channel::handleEvent(base::TimeStamp receiveTime)
		{
			std::shared_ptr<void> guard;
			if (tied_)
			{
				guard = tie_.lock();
				if (guard)
				{
					handleEventWithGuard(receiveTime);
				}
			}else{
				handleEventWithGuard(receiveTime);
			}

		}

		void Channel::tie(const std::shared_ptr<void>& obj)
		{
			tie_ = obj;
			tied_ = true;
		}



		std::string Channel::eventsToString() const
		{
			return eventsToString(fd_, events_);
		}

		std::string Channel::reventsToString() const
		{
			return eventsToString(fd_, revents_);
		}

		std::string Channel::eventsToString(int fd, int ev)
		{
			std::ostringstream oss;
			oss << fd << ": ";
			if (ev & POLLIN)
				oss << "IN ";
			if (ev & POLLPRI)
				oss << "PRI ";
			if (ev & POLLOUT)
				oss << "OUT ";
			if (ev & POLLHUP) //表示对应的文件描述符被挂断；
				oss << "HUP ";
			if (ev & POLLRDHUP)
				oss << "RDHUP ";
			if (ev & POLLERR)
				oss << "ERR ";
			if (ev & POLLNVAL)
				oss << "NVAL ";

			return oss.str();
		}

		void Channel::handleEventWithGuard(base::TimeStamp receiveTime)
		{
			eventHandling_ = true;
			LOG_TRACE << reventsToString();

			if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
			{
				if (logHup_)
				{
					LOG_ERROR << "FD = " << fd_ << "channel::handle_event POOLHUP";
				}
				if (closeCallback_) closeCallback_();
			}

			if (revents_ & POLLNVAL)
			{
				LOG_ERROR << "fd = " << fd_ << " Channel::handle_event() POLLNVAL";
			}

			if (revents_ & (POLLERR | POLLNVAL))
			{
				if (errorCallback_) errorCallback_();
			}

			if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
			{
				if (readCallBack_) readCallBack_(receiveTime);
			}

			if (revents_ & POLLOUT)
			{
				if(writeCallback_) writeCallback_();
			}

			eventHandling_ = false;
		}



	}
}