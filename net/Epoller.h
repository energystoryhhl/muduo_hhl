#ifndef EPOLLER_H
#define EPOLLER_H

#include "poller.h"

//#include "EventLoop.h"
//#include "poll.h"
//#include "sys/epoll.h"

//#include "Logging.h"
//
//#include <unistd.h>
//#include <assert.h>
//
//#include <memory>

//#include "sys/epoll.h"

struct epoll_event;

namespace hhl {
	namespace net {
		
		

		class Epoller : public Poller
		{
		public:
			Epoller(EventLoop * eventLoop);
			~Epoller() override;

			base::TimeStamp poll(int timeoutMs, ChannelList* activeChannels) override;

			void update(int op, Channel * channel);

			void updateChannel(Channel * channel) override;

			void removeChannel(Channel * channel) override;

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

