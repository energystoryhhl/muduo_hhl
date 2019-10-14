#ifndef INETADDRES_H
#define INETADDRES_H

#include <netinet/in.h>
#include <string>

namespace hhl
{
	namespace net
	{
		namespace sockets 
		{
			const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
		}


		class InetAddress
		{
		public:
			explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

			InetAddress(std::string ip, uint16_t port, bool ipv6 = false);

			explicit InetAddress(const struct sockaddr_in& addr)
				:addr_(addr)
			{
			}

			explicit InetAddress(const struct sockaddr_in6& addr)
				:addr6_(addr)
			{
			}

			sa_family_t family() const { return addr_.sin_family; }

			std::string toIp() const;

			std::string tpIPPort() const;

			std::string tpIP() const;

			const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }

			void setSockAddrInet6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

			static bool resolve(std::string hostname, InetAddress* out);

		private:
			struct sockaddr_in addr_;
			struct sockaddr_in6 addr6_;


		};




	}
}










#endif // !INETADDRES_H

