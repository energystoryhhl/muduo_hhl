#ifndef CHANNEL_H
#define CHANNEL_H

#include "noncopyable.h"
#include "TimeStamp.h"
#include "EventLoop.h"

#include <memory>

namespace hhl
{
	namespace net
	{
		class Channel : public noncopyable
		{

		public:
			typedef std::function<void()> EventCallback;
			typedef std::function<void(hhl::base::TimeStamp)> ReadEventCallback;

			Channel(EventLoop * loop, int fd);
			~Channel();

			void handleEvent(base::TimeStamp receiveTime);
			
			void setReadCallback(ReadEventCallback cb)
			{
				readCallBack_ = std::move(cb);
			}

			void setWriteCallback(EventCallback cb)
			{
				writeCallback_ = std::move(cb);
			}

			void setCloseCallback(EventCallback cb)
			{
				closeCallback_ = std::move(cb);
			}

			void setErrorCallback(EventCallback cb)
			{
				errorCallback_ = std::move(cb);
			}

			EventLoop * ownerLoop() { return loop_; };

			void tie(const std::shared_ptr<void> & obj);

			int fd() const { return fd_; }

			int events() const { return events_; }

			void set_revents(int revt) { revents_ = revt; }

			bool isNoneEvent() const { return events_ == kNoneEvent; }

			//void enableReading()

			void update();

			void remove();

			//poller
			int index() { return index_; }

			void set_index(int idx) { index_ = idx; }

			std::string eventsToString() const;

			std::string reventsToString() const;

			void enableReading() { events_ |= kReadEvent; update(); }
			void disableReading() { events_ &= ~kReadEvent; update(); }
			void enableWriting() { events_ |= kWriteEvent; update(); }
			void disableWriting() { events_ &= ~kWriteEvent; update(); }
			void disableAll() { events_ = kNoneEvent; update(); }
			bool isWriting() const { return events_ & kWriteEvent; }
			bool isReading() const { return events_ & kReadEvent; }

		private:

			static std::string eventsToString(int fd, int ev);

			void handleEventWithGuard(base::TimeStamp receiveTime);

			static const int kNoneEvent;
			static const int kReadEvent;
			static const int kWriteEvent;

			EventLoop * loop_;
			const int fd_;
			int events_;
			int revents_;
			int index_;
			bool logHup_;

			std::weak_ptr<void> tie_;
			bool tied_;
			bool eventHandling_;
			bool addedToLoop_;
			ReadEventCallback readCallBack_;
			EventCallback writeCallback_;
			EventCallback closeCallback_;
			EventCallback errorCallback_;

		};
	}//namespace net
}//namespace hhl
#endif // !CHANNEL_H

