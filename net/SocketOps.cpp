#include "SocketOps.h"
#include "Endian.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "Logging.h"


namespace hhl
{
	namespace net 
	{
		namespace sockets
		{

			void fromIpPort(const char * ip, uint16_t port, sockaddr_in6 * addr)
			{
				addr->sin6_family = AF_INET6;
				addr->sin6_port = hostToNetwork16(port);
				if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
				{
					LOG_DEBUG << "sockets::fromIpPort";
				}
			}
			void fromIpPort(const char * ip, uint16_t port, sockaddr_in * addr)
			{
				addr->sin_family = AF_INET;
				addr->sin_port = hostToNetwork16(port);
				if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
				{
					LOG_DEBUG << "sockets::fromIpPort";
				}
			}
			const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr)
			{
				return (const struct sockaddr*)((addr));
			}

			struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr)
			{
				return (struct sockaddr*)((addr));
			}

			const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr)
			{
				return (const struct sockaddr*)(addr);
			}

			const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr)
			{
				return (const struct sockaddr_in*)(addr);
			}

			const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr)
			{
				return (const struct sockaddr_in6*)(addr);
			}

		}

		void sockets::toIp(char* buf, size_t size, const struct sockaddr* addr)
		{
			if (addr->sa_family == AF_INET)
			{
				assert(size >= INET_ADDRSTRLEN);
				const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
				::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
			}
			else if (addr->sa_family == AF_INET6)
			{
				assert(size >= INET6_ADDRSTRLEN);
				const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
				::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
			}
		}
		void sockets::toIpPort(char* buf, size_t size,
			const struct sockaddr* addr)
		{
			toIp(buf, size, addr);
			size_t end = ::strlen(buf);
			const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
			uint16_t port = sockets::networkToHost16(addr4->sin_port);
			assert(size > end);
			snprintf(buf + end, size - end, ":%u", port);
		}

		void sockets::bindOrDie(int sockfd, const sockaddr * addr)
		{
			int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sizeof(struct sockaddr_in6))));
			if (ret < 0)
			{
				LOG_DEBUG << "sockets::bindOrDie";
			}
		}

		void sockets::listenOrDie(int sockfd)
		{
			int ret = ::listen(sockfd, SOMAXCONN);
			if (ret < 0)
			{
				LOG_DEBUG << "sockets::listenOrDie";
			}

		}

		int sockets::accept(int sockfd, sockaddr_in6 * addr)
		{
			socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));


			#if VALGRIND || defined (NO_ACCEPT4)
			int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
			setNonBlockAndCloseOnExec(connfd);
			#else
			int connfd = ::accept4(sockfd, sockaddr_cast(addr),
				&addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
			#endif

			if (connfd < 0)
			{
				int savedErrno = errno;
				LOG_DEBUG << "Socket::accept";
				switch (savedErrno)
				{
				case EAGAIN:
				case ECONNABORTED:
				case EINTR:
				case EPROTO: // ???
				case EPERM:
				case EMFILE: // per-process lmit of open file desctiptor ???
				  // expected errors
					errno = savedErrno;
					break;
				case EBADF:
				case EFAULT:
				case EINVAL:
				case ENFILE:
				case ENOBUFS:
				case ENOMEM:
				case ENOTSOCK:
				case EOPNOTSUPP:
					// unexpected errors
					LOG_DEBUG << "unexpected error of ::accept " << savedErrno;
					break;
				default:
					LOG_DEBUG << "unknown error of ::accept " << savedErrno;
					break;
				}
			}
			return connfd;
		}

		void sockets::shutdownWrite(int sockfd)
		{
			if (::shutdown(sockfd, SHUT_RD) < 0)
			{
				LOG_DEBUG << "sockets::shutdownWrite";
			}
		}





	}
}



