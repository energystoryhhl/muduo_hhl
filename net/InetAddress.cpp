#include "net/InetAddress.h"

#include <memory.h>
#include <string.h>
#include <assert.h>
#include <netdb.h>

#include "SocketOps.h"
#include "Endian.h"
#include "Logging.h"
namespace hhl
{
	namespace net
	{

		InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
		{
			if (ipv6)
			{
				memset(&addr6_, 0, sizeof(addr6_));
				addr6_.sin6_family = AF_INET6;
				in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
				addr6_.sin6_addr = ip;
				addr6_.sin6_port = sockets::hostToNetwork16(port);
			}
			else
			{
				memset(&addr_, 0, sizeof(addr_));
				addr_.sin_family = AF_INET;
				in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
				addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
				addr_.sin_port = sockets::hostToNetwork16(port);
			}
		}

		InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6)
		{
			if (ipv6)
			{
				memset(&addr6_, 0, sizeof(addr6_));
				sockets::fromIpPort(ip.c_str(), port, &addr6_);
			}
			else
			{
				memset(&addr_, 0, sizeof(addr_));
				sockets::fromIpPort(ip.c_str(), port, &addr_);
			}
		}

		std::string InetAddress::toIp() const
		{
			char buf[64] = "";
			sockets::toIp(buf, sizeof buf, getSockAddr());
			return buf;
		}

		std::string InetAddress::tpIPPort() const
		{
			char buf[64] = "";
			sockets::toIpPort(buf, sizeof buf, getSockAddr());
			return buf;
		}

		std::string InetAddress::tpIP() const
		{
			char buf[64] = "";
			sockets::toIp(buf, sizeof buf, getSockAddr());
			return buf;
		}

		static __thread char t_resolveBuffer[64 * 1024];

		bool InetAddress::resolve(std::string hostname, InetAddress * out)
		{
			assert(out != NULL);
			struct hostent hent;
			struct hostent*  he = NULL;
			int herrno = 0;
			memset(&hent, 0, sizeof(hent));

			int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof(t_resolveBuffer), &he, &herrno);

			if (ret == 0 && he != NULL)
			{
				assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
				out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
				return true;
			}
			else
			{
				if (ret)
				{
					LOG_DEBUG << "InetAddress::resolve";
				}
				return false;
			}

		}

	}
}