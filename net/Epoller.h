#ifndef EPOLLER_H
#define EPOLLER_H

#include "poller.h"
#include "EventLoop.h"
#include "poll.h"
#include "sys/epoll.h"

#include "Logging.h"

#include <unistd.h>
#include <assert.h>

#include <memory>


namespace hhl {
	namespace net {
		
		class Epoller : public Poller
		{
		public:
			Epoller(EventLoop * eventLoop);
			~Epoller() override;

			void update(int op, Channel * channel);

			void updateChannel(Channel * channel);

			void removeChannel(Channel * channel);
			void fillActiveChannels(int numEvents,
				ChannelList* activeChannels) const;
		private:

			static const int KInitEventListSize = 16;

			static const char * operationToString(int op);

			typedef std::vector<struct epoll_event> EventList;

			int epollfd_;

			EventList events_;



		};

	}//namespace net
}

#endif // !EPOLLER_H

