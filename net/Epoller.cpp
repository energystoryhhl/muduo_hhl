#include "Epoller.h"



namespace hhl{
	namespace net {
		const int KNew = -1;
		const int KAdded = 1;
		const int KDeleted = 2;

		Epoller::Epoller(EventLoop * eventLoop)
			:Poller(eventLoop),
			epollfd_(epoll_create1(EPOLL_CLOEXEC)),  //init epoll
			events_(KInitEventListSize)
		{
			if (epollfd_ < 0)
			{
				LOG_ERROR << "CREATE EPOLL FAILED!\n";
			}
		}

		Epoller::~Epoller()
		{
			::close(epollfd_);
		}

		void Epoller::fillActiveChannels(int numEvents,
			ChannelList* activeChannels) const
		{
			assert(static_cast<size_t>(numEvents) <= events_.size());
			for (int i = 0; i < numEvents; ++i)
			{
				Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
				int fd = channel->fd();
				ChannelMap::const_iterator it = channels_.find(fd);
				assert(it != channels_.end());
				assert(it->second == channel);
#endif
				channel->set_revents(events_[i].events);
				activeChannels->push_back(channel);
			}
		}

		void Epoller::updateChannel(Channel * channel)
		{
			const int index = channel->index();
			LOG_TRACE << "fd = " << channel->fd() << "index = " << index << "events = " << channel->events();
			if (index == KNew || index == KDeleted)
			{
				int fd = channel->fd();

				if (channels_.find(fd) == channels_.end())	//Not found in map
				{
					channels_[fd] = channel;				//Add
				}
				else if(channels_.find(fd) != channels_.end())	//Not found in map
				{
					assert(channels_.find(fd) != channels_.end());
					assert(channels_[fd] == channel);
				}
				channel->set_index(KAdded);
				update(EPOLL_CTL_ADD, channel);
			}else {
				int fd = channel->fd();
				(void)fd;
				assert(channels_.find(fd) != channels_.end());
				assert(channels_[fd] == channel);
				assert(index == KAdded);
				if (channel->isNoneEvent())
				{
					update(EPOLL_CTL_DEL, channel);
					channel->set_index(KDeleted);
				}
				else
				{
					update(EPOLL_CTL_MOD, channel);
				}

			}


		}

		void Epoller::removeChannel(Channel * channel)
		{
			int fd = channel->fd();
			LOG_DEBUG << "fd = " << fd;
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
			assert(channel->isNoneEvent());
			int index = channel->index();
			assert(index == KAdded || index == KDeleted);
			size_t n = channels_.erase(fd);

			(void)n;

			assert(n == 1);

			if (index == KAdded)
			{
				update(EPOLL_CTL_DEL, channel);
			}
			channel->set_index(KNew);
		}



		void Epoller::update(int operation, Channel * channel)
		{
			struct epoll_event event;
			memset(&event, 0, sizeof(event));		
			event.events = channel->events();		//set epoll events: read write 
			event.data.ptr = channel;
			int fd = channel->fd();

			LOG_TRACE << "epoll_ctl op = " << operationToString(operation)
				<< " fd = " << fd << " event = { " << channel->eventsToString() << " }";

			if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) //add epoll events
			{
				LOG_ERROR << "opoll_ctl error op = " << operationToString(operation) << "fd = " << fd;
			}
		}


		const char* Epoller::operationToString(int op)
		{
			switch (op)
			{
			case EPOLL_CTL_ADD:
					return "ADD";
			case EPOLL_CTL_DEL:
				return "DEL";
			case EPOLL_CTL_MOD:
				return "MOD";
			default:
				return "ERR OPERATION";

			}
		}

	}
}