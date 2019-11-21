#include "TcpClient.h"
#include "net/SocketOps.h"
#include "net/EventLoop.h"

using namespace hhl;
using namespace hhl::net;

TcpClient::TcpClient(EventLoop * loop, const net::InetAddress & serverAddr,const std::string & name)
	:
	loop_(loop),
	connector_(new Connector(loop_,serverAddr)),
	name_(name),
	connectionCallback_(defaultConnectionCallback),
	messageCallback_(defaultMessageCallback),
	retry_(false),
	connect_(true),
	nextConnId_(1)
{
	connector_->setNewConnectionCallback(
		std::bind(&TcpClient::newConnection, this, std::placeholders::_1)
	
	);
	LOG_DEBUG << "TcpClient::TcpClient[" << name_
		<< "] - connector" << get_pointer(connector_);

}


void hhl::net::TcpClient::connect()
{
	// FIXME: check state
	LOG_DEBUG << "TcpClient::connect[" << name_ << "] - connecting to "
		<< connector_->serverAddr().tpIPPort();
	connect_ = true;
	connector_->start();
}

void hhl::net::TcpClient::disconnect()
{
	connect_ = false;
	{
		MutexLockGuard lock(mutex_);
		if (connection_)
		{
			connection_->shutdown();
		}
	}
}

TcpClient::~TcpClient()
{
	LOG_DEBUG << "TcpClient::~TcpClient[" << name_
		<< "] - connector " << get_pointer(connector_);
	TcpConnectionPtr conn;
	bool unique = false;
	{
		MutexLockGuard lock(mutex_);
		unique = connection_.unique();
		conn = connection_;
	}
	if (conn)
	{
		assert(loop_ == conn->getLoop());
		// FIXME: not 100% safe, if we are in different thread
		CloseCallback cb = std::bind(&sockets::removeConnection, loop_, _1);
		loop_->runInLoop(
			std::bind(&TcpConnection::setCloseCallback, conn, cb));
		if (unique)
		{
			conn->forceClose();
		}
	}
	else
	{
		connector_->stop();
		// FIXME: HACK
		loop_->runAfter(1, std::bind(&sockets::removeConnector, connector_));
	}
}

void TcpClient::stop()
{
	connect_ = false;
	connector_->stop();
}


void TcpClient::newConnection(int sockfd)
{
	loop_->assertInLoopThread();
	InetAddress	peerAddr(sockets::getPeerAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof buf, ":%s#%d", peerAddr.tpIPPort().c_str());
	++nextConnId_;
	string connName = name_ + buf;

	InetAddress localAddr(sockets::getLocalAddr(sockfd));

	TcpConnectionPtr conn(new TcpConnection(loop_,
		connName,
		sockfd,
		localAddr,
		peerAddr
	));
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
		std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));

	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}
	conn->connectEstablished();
}

void hhl::net::TcpClient::removeConnection(const TcpConnectionPtr & conn)
{
	loop_->assertInLoopThread();
	assert(loop_ == conn->getLoop());
	{
		MutexLockGuard lock(mutex_);
		assert(connection_ == conn);
		connection_.reset();
	}
}
