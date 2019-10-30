#ifndef SOCKET_H
#define SOCKET_H

#include "noncopyable.h"
#include "net/InetAddress.h"
#include "sys/socket.h"
#include <netinet/tcp.h>


namespace hhl
{
	namespace net 
	{
		class InetAddress;

		class Socket :  noncopyable
		{
		public:
			explicit Socket(int sockfd)
				:
				sockfd_(sockfd)
			{}


			~Socket();

			int fd() const { return sockfd_; }

			bool getTcpInfo(struct tcp_info* tcpi) const;

			bool getTcpInfoString(char* buf, int len) const;

			void bindAddress(const InetAddress& localaddr);

			void listen();

			int accept(InetAddress* peeraddr);

			void shutdownWrite();

			void setTcpNoDelay(bool on);

			void setKeepAlive(bool on);

		private:
			int sockfd_;
		};

	}
}



















#endif // !SOCKET_H

