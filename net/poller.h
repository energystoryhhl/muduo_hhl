#ifndef POLLER_H
#define POLLER_H

#include "poller.h"
#include  "noncopyable.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TimeStamp.h"

#include <vector>
#include <map>


namespace hhl 
{
	namespace net
	{

		class Poller: noncopyable
		{
		public:
			typedef std::vector<Channel*> ChannelList;

			Poller(EventLoop* loop);

			virtual ~Poller();

			virtual base::TimeStamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

			virtual void updateChannel(Channel * channel) = 0;

			virtual void removeChannel(Channel * channel) = 0;

			virtual bool hasChannel(Channel * channel) const;

			static Poller* newDefaultPoller(EventLoop * loop);

			void assertInLoopThread() const
			{
				//ownerLoop_->assertInLoopThread(); //todo
			}

		protected:
			typedef std::map<int, Channel *> ChannelMap;

			ChannelMap channels_;

		private:
			EventLoop * ownerLoop_;

		};



	}//	namespace net
}



#endif // !POLLER_H

