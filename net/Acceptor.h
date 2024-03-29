#pragma once
#include <functional>

#include "noncopyable.h"
#include "net/Socket.h"
#include "net/Channel.h"


namespace hhl
{
	namespace net 
	{
		class EventLoop;
		class InetAddress;

		class Acceptor : noncopyable
		{
		public:
			typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

			Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
			~Acceptor();

			void setNewConnectionCallback(const NewConnectionCallback& cb)
			{
				newConnectionCallback_ = cb;
			}

			bool listenning() const { return listenning_; }
			void listen();


		private:
			void handleRead();
			

			EventLoop*			loop_;
			Socket				acceptSocket_;
			Channel				acceptChannel_;
			NewConnectionCallback	newConnectionCallback_;
			bool				listenning_;
			int					idleFd_;
		};
	}
}