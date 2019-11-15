#include "net/TcpServer.h"
#include "net/Acceptor.h"
#include "net/EventLoopThreadPool.h"

using namespace hhl;
using namespace hhl::net;

TcpServer::TcpServer(EventLoop * loop,
	const InetAddress & listenAddr,
	const string & nameArg,
	Option option)
	:
	loop_(loop),
	ipPort_(listenAddr.tpIPPort()),
	name_(nameArg),
	accepter_(new Acceptor(loop, listenAddr, option == kReusePort)),
	threadPool_(new EventLoopThreadPool(loop, name_)),
	connectionCallback_(defaultConnectionCallback),
	messageCallback_(defaultMessageCallback),
	nextConnId_(1)
{
	



}
