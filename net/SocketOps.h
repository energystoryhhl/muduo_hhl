#ifndef SOCKETOPS_H
#define SOCKETOPS_H

#include <stdint.h>
#include <netinet/in.h>
#include "net/Connector.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/TcpConnection.h"

#include <memory>

namespace hhl
{
	namespace net
	{
		namespace sockets
		{
			int createNonblockingOrDie(sa_family_t family);

			ssize_t write(int sockfd, const void *buf, size_t count);

			void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in6* addr);

			void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in* addr);

			const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);

			struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);

			const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

			const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);

			void toIp(char* buf, size_t size, const struct sockaddr* addr);

			const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

			void toIpPort(char* buf, size_t size, const struct sockaddr* addr);

			void bindOrDie(int sockfd, const struct sockaddr* addr);

			void listenOrDie(int sockfd);

			int accept(int sockfd, struct sockaddr_in6* addr);

			void shutdownWrite(int sockfd);

			ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);

			int getSocketError(int sockfd);

			void close(int sockfd);

			int connect(int sockfd, const struct sockaddr* addr);

			void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn);

			void removeConnector(shared_ptr< Connector> c);

			bool isSelfConnect(int sockfd);

			struct sockaddr_in6 getLocalAddr(int sockfd);

			struct sockaddr_in6 getPeerAddr(int sockfd);
		}
	}
}



#endif // !SOCKETOPS_H

