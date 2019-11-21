#include "net/TcpServer.h"
#include "net/Acceptor.h"
#include "net/EventLoopThreadPool.h"
#include "net/SocketOps.h"

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
	accepter_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, \
		std::placeholders::_1, std::placeholders::_2));



}

void TcpServer::newConnection(int sockfd, const InetAddress & peerAddr)
{
	loop_->assertInLoopThread();
	EventLoop* ioLoop = threadPool_->getNextLoop();
	char buf[64];
	snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
	++nextConnId_;
	string connName = name_ + buf;

	LOG_DEBUG << "TcpServer::newConnection [" << name_
		<< "] - new connection [" << connName
		<< "] from " << peerAddr.tpIPPort();

	InetAddress localAddr(sockets::getLocalAddr(sockfd));

	TcpConnectionPtr conn(new TcpConnection(ioLoop,
		connName,
		sockfd,
		localAddr,
		peerAddr
	));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
		std::bind(&TcpServer::removeConnection,this, std::placeholders::_1));
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));

}

void hhl::net::TcpServer::setThreadNum(int numThreads)
{
	assert(0 <= numThreads);
	threadPool_->setThreadNum(numThreads);
}

void hhl::net::TcpServer::start()
{
	if (started_.getAndSet(1) == 0)
	{
		threadPool_->start(threadInitCallback_);

		assert(!accepter_->listenning());
		loop_->runInLoop(
			std::bind(&Acceptor::listen, get_pointer(accepter_)));
	}
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	// FIXME: unsafe
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();
	LOG_DEBUG << "TcpServer::removeConnectionInLoop [" << name_
		<< "] - connection " << conn->name();
	size_t n = connections_.erase(conn->name());
	(void)n;
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queneInLoop(
		std::bind(&TcpConnection::connectDestroyed, conn));
}
