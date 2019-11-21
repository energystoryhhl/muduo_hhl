#pragma once

#include "net/InetAddress.h"

#include "net/Connector.h"
#include "net/TcpConnection.h"

#include <memory>
#include <string>


namespace hhl
{
	namespace net
	{
		class EventLoop;

		class TcpClient : noncopyable
		{
		public:

			TcpClient(EventLoop* loop,
				 const InetAddress& serverAddr, 
					const string& name);
			~TcpClient();  // force out-line dtor, for std::unique_ptr members.
			typedef std::shared_ptr<Connector> ConnectorPtr;

			void connect();
			void disconnect();
			void stop();

			TcpConnectionPtr connection() const
			{
				MutexLockGuard lock(mutex_);
				return connection_;
			}

			EventLoop* getLoop() const { return loop_; }
			bool retry() const { return retry_; }
			void enableRetry() { retry_ = true; }
			const string& name() const
			{
				return name_;
			}

			/// Set connection callback.
			/// Not thread safe.
			void setConnectionCallback(ConnectionCallback cb)
			{
				connectionCallback_ = std::move(cb);
			}

			/// Set message callback.
			/// Not thread safe.
			void setMessageCallback(MessageCallback cb)
			{
				messageCallback_ = std::move(cb);
			}

			/// Set write complete callback.
			/// Not thread safe.
			void setWriteCompleteCallback(WriteCompleteCallback cb)
			{
				writeCompleteCallback_ = std::move(cb);
			}

			void newConnection(int sockfd);

		private:

			void removeConnection(const TcpConnectionPtr& conn);

			EventLoop*					loop_;
			ConnectorPtr				connector_;
			const std::string			name_;
			ConnectionCallback			connectionCallback_;
			MessageCallback				messageCallback_;
			WriteCompleteCallback		writeCompleteCallback_;
			bool						retry_;   // atomic
			bool						connect_; // atomic
			// always in loop thread
			int							nextConnId_;
			mutable MutexLock			mutex_;
			TcpConnectionPtr			connection_ ;
		};




	}
}