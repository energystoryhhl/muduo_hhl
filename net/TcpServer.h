#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "base/Atomic.h"
#include "base/types.h"
#include "net/TcpConnection.h"

#include <functional>
#include <map>
#include "net/InetAddress.h"
#include "memory"

namespace hhl
{
	namespace net
	{
		class Acceptor;
		class EventLoop;
		class EventLoopThreadPool;
		//class EventLoop

		class TcpServer : noncopyable
		{
		public:

			typedef std::function<void(EventLoop*)> ThreadInitCallback;
			enum Option
			{
				kNoReusePort,
				kReusePort,
			};

			TcpServer(EventLoop* loop,
				const InetAddress& listenAddr,
				const string& nameArg,
				Option option = kNoReusePort
			);
				


		private:

			typedef std::map<string, TcpConnectionPtr> ConnectionMap;

			EventLoop*								loop_;
			const std::string						ipPort_;
			const std::string						name_;
			std::unique_ptr<Acceptor>				accepter_;
			std::shared_ptr<EventLoopThreadPool>	threadPool_;
			ConnectionCallback						connectionCallback_;
			MessageCallback							messageCallback_;
			WriteCompleteCallback					writeCompleteCallback_;
			ThreadInitCallback						threadInitCallback_;
			AtomicInt32								started_;

			int										nextConnId_;
			ConnectionMap							connections_;


		};


	}
}

#endif // !TCPSERVER_H
