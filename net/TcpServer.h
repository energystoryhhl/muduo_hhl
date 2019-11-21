#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "base/Atomic.h"
#include "base/types.h"
#include "net/TcpConnection.h"

#include <functional>
#include <map>
#include "net/InetAddress.h"
#include "net/Acceptor.h"
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
			
			const string& ipPort() const { return ipPort_; }
			
			const string& name() const { return name_; }

			EventLoop* getLoop() const { return loop_; }

			void setThreadNum(int numThreads);

			void setThreadInitCallback(const ThreadInitCallback& cb)
			{
				threadInitCallback_ = cb;
			}

			std::shared_ptr<EventLoopThreadPool> threadPool()
			{
				return threadPool_;
			}

			void start();

			void removeConnection(const TcpConnectionPtr& conn);

			void setConnectionCallback(const ConnectionCallback& cb)
			{
				connectionCallback_ = cb;
			}

			void setMessageCallback(const MessageCallback& cb)
			{
				messageCallback_ = cb;
			}

			void setWriteCompleteCallback(const WriteCompleteCallback& cb)
			{
				writeCompleteCallback_ = cb;
			}

			//void removeConnectionInLoop(const TcpConnectionPtr& conn);

		private:
			void newConnection(int sockfd, const InetAddress& peerAddr);
			void removeConnectionInLoop(const TcpConnectionPtr& conn);

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
