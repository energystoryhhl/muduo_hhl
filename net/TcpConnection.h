#pragma once

#include <errno.h>
#include <string>
#include <memory>


#include "noncopyable.h"
#include "InetAddress.h"
#include "net/Callbacks.h"
#include "Logging.h"
#include "net/Channel.h"
#include "net/Socket.h"

using namespace std;

namespace hhl
{
	namespace net
	{
		class Channel;
		class EventLoop;
		class Socket;

		class TcpConnection : noncopyable , public std::enable_shared_from_this<TcpConnection>
		{
		public:
			TcpConnection(EventLoop* loop,
				const std::string& name,
				int sockfd,
				const InetAddress& localAddr,
				const InetAddress& peerAddr
			);

			~TcpConnection();


			EventLoop* getLoop() const { return loop_; }
			const string& name() const { return name_; }
			const InetAddress& localAddress() const { return localAddr_; }
			const InetAddress& peerAddress() const { return peerAddr_; }
			bool connected() const { return state_ == kConnected; }
			bool disconected() const { return state_ == kDisconnected; }

			bool getTcpInfo(struct tcp_info* tcpi) const;
			string getTcpInfoString() const;

			void send(const void* message, int len);
			void send(const std::string message);

			void send(Buffer* message);

			void shutdown(); // NOT thread safe, no simultaneous calling
			// void shutdownAndForceCloseAfter(double seconds); // NOT thread safe, no simultaneous calling
			void forceClose();
			//void forceCloseWithDelay(double seconds);
			void setTcpNoDelay(bool on);

			void startRead();
			void stopRead();
			bool isReading() const { return reading_; };


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

			void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
			{
				highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark;
			}
			/// Advanced interface
			Buffer* inputBuffer()
			{
				return &inputBuffer_;
			}

			Buffer* outputBuffer()
			{
				return &outputBuffer_;
			}







		private:
			void sendInLoop(const void* message, size_t len);
			void sendInLoop(const std::string& mesg);
			void handleRead(base::TimeStamp receiveTime);
			void handleWrite();
			void handleClose();
			void handleError();
			const char* stateToString() const;	

			void shutdownInLoop();
			void forceCloseInLoop();
			void startReadInLoop();
			void stopReadInLoop();
			
			enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };

			void setState(StateE s) { state_ = s; }
			EventLoop* loop_;
			const string name_;
			StateE state_;  // FIXME: use atomic variable
			bool reading_;

			std::unique_ptr<Socket> socket_;
			std::unique_ptr<Channel> channel_;
			const InetAddress localAddr_;
			const InetAddress peerAddr_;
			ConnectionCallback connectionCallback_;
			MessageCallback messageCallback_;
			WriteCompleteCallback writeCompleteCallback_;
			HighWaterMarkCallback highWaterMarkCallback_;
			CloseCallback closeCallback_;
			size_t highWaterMark_;
			Buffer inputBuffer_;
			Buffer outputBuffer_; 

		};


	}
}
