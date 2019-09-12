#include "poller.h"
#include "Channel.h"
#include "Epoller.h"

namespace hhl
{
	namespace net
	{

		Poller::Poller(EventLoop * loop)
			:ownerLoop_(loop)
		{

		}

		Poller::~Poller() = default;

		bool Poller::hasChannel(Channel * channel) const
		{
			assertInLoopThread();
			ChannelMap::const_iterator it = channels_.find(channel->fd());
			return it != channels_.end() && it->second == channel;
		}

		Poller * Poller::newDefaultPoller(EventLoop * loop)
		{
			if (getenv("MUDUO_USE_POLL"))
			{
				
			}
			else {
				return new Epoller(loop);
			}
		}


	}
}