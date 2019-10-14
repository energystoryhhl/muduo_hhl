#ifndef SOCKETOPS_H
#define SOCKETOPS_H

#include <stdint.h>
#include <netinet/in.h>


namespace hhl
{
	namespace net
	{
		namespace sockets
		{
			void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in6* addr);

			void fromIpPort(const char *ip, uint16_t port, struct sockaddr_in* addr);

			const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);

			struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);

			const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

			const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);

			void toIp(char* buf, size_t size, const struct sockaddr* addr);

			const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

			void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
		}
	}
}



#endif // !SOCKETOPS_H

