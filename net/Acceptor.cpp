#include "Acceptor.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
#include "net/SocketOps.h"
#include "Logging.h"

#include <unistd.h>
#include <fcntl.h>

using namespace hhl;
using namespace hhl::net;

Acceptor::Acceptor(EventLoop * loop, const InetAddress & listenAddr, bool reuseport)
	:
	loop_(loop),
	acceptSocket_( sockets::createNonblockingOrDie(listenAddr.family()) ),
	acceptChannel_(loop, acceptSocket_.fd()),
	listenning_(false),
	idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(
	std::bind(&Acceptor::handleRead, this));
}

void Acceptor::handleRead()
{
	loop_->assertInLoopThread();
	InetAddress peerAddr;
	//FIXME loop until no more
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0)
	{
		// string hostport = peerAddr.toIpPort();
		// LOG_TRACE << "Accepts of " << hostport;
		if (newConnectionCallback_)
		{
			//todo!!!!
			newConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
	else
	{
		LOG_DEBUG << "in Acceptor::handleRead";
		// Read the section named "The special problem of
		// accept()ing when you can't" in libev's doc.
		// By Marc Lehmann, author of libev.
		if (errno == EMFILE)
		{
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}



Acceptor::~Acceptor()
{
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}


void Acceptor::listen()
{
	loop_->assertInLoopThread();
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}