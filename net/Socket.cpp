#include "net/Socket.h"
#include "unistd.h"
#include "memory.h"

namespace hhl
{
	namespace net
	{

		bool Socket::getTcpInfo(tcp_info *tcpi) const
		{
			socklen_t len = sizeof(*tcpi);
			memset(tcpi, 0,len);
			return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
		}

		bool Socket::getTcpInfoString(char* buf, int len) const
		{
			struct tcp_info tcpi;
			bool ok = getTcpInfo(&tcpi);
			if (ok)
			{
				snprintf(buf, len, "unrecovered=%u "
					"rto=%u ato=%u snd_mss=%u rcv_mss=%u "
					"lost=%u retrans=%u rtt=%u rttvar=%u "
					"sshthresh=%u cwnd=%u total_retrans=%u",
					tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
					tcpi.tcpi_rto,          // Retransmit timeout in usec
					tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
					tcpi.tcpi_snd_mss,
					tcpi.tcpi_rcv_mss,
					tcpi.tcpi_lost,         // Lost packets
					tcpi.tcpi_retrans,      // Retransmitted packets out
					tcpi.tcpi_rtt,          // Smoothed round trip time in usec
					tcpi.tcpi_rttvar,       // Medium deviation
					tcpi.tcpi_snd_ssthresh,
					tcpi.tcpi_snd_cwnd,
					tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
			}
			return ok;
		}

	}
}