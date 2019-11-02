#include "TcpConnection.h"
#include "Logging.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Socket.h"
#include "net/SocketOps.h"
#include "base/TimeStamp.h"

using namespace hhl;
using namespace hhl::net;
using namespace hhl::base;

TcpConnection::TcpConnection(EventLoop * loop,
	const std::string & name,
	int sockfd,
	const InetAddress & localAddr,
	const InetAddress & peerAddr)
	:
	loop_(loop),
	name_(name),
	state_(kConnecting),
	reading_(true),
	socket_(new hhl::net::Socket(sockfd)),
	channel_(new Channel(loop, sockfd)),
	localAddr_(localAddr),
	peerAddr_(peerAddr),
	highWaterMark_(64 * 1024 * 1024)
{
	channel_->setReadCallback(
		std::bind(&TcpConnection::handleRead, this, _1)
		);

	channel_->setWriteCallback(
		std::bind(&TcpConnection::handleWrite, this));
	
	channel_->setCloseCallback(
		std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(
		std::bind(&TcpConnection::handleError, this));

	LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
		<< " fd=" << sockfd;

	socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
		LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
			<< " fd=" << channel_->fd()
			<< " state="<< stateToString();
		assert(state_ == kDisconnected);
}

bool hhl::net::TcpConnection::getTcpInfo(tcp_info * tcpi) const
{
	return socket_->getTcpInfo(tcpi);
}

string hhl::net::TcpConnection::getTcpInfoString() const
{
	char buf[1024] = { 0 };
	buf[0] = '\0';
	socket_->getTcpInfoString(buf, sizeof(buf));
	return buf;
}

void hhl::net::TcpConnection::send(const void * message, int len)
{
	return sendInLoop(message, len);
}

void hhl::net::TcpConnection::send(const std::string message)
{
	if (state_ == kConnected)
	{
		if (loop_->isInLoopThread())
		{
			sendInLoop(message);
		}
		else {
			void (TcpConnection::*fp)(const std::string& msg) = &TcpConnection::sendInLoop;
			loop_->runInLoop(
				std::bind(fp, this, message));
		}
	}

}

void hhl::net::TcpConnection::send(Buffer * message)
{
	if (state_ == kConnected)
	{
		if (loop_->isInLoopThread())
		{
			sendInLoop(message->peek(), message->readableBytes());
			message->retrieveAll();
		}
		else
		{
			void (TcpConnection::*fp)(const std::string& msg) = &TcpConnection::sendInLoop;

			loop_->runInLoop(
				std::bind(fp, this, message->retrieveAllAsString() ));
		}
	}
}

void hhl::net::TcpConnection::shutdown()
{
	// FIXME: use compare and swap
	if (state_ == kConnected)
	{
		setState(kDisconnecting);
		// FIXME: shared_from_this()?
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}


}

void hhl::net::TcpConnection::forceClose()
{
	if (state_ == kConnected || state_ == kDisconnecting)
	{
		setState(kDisconnecting);
		loop_->queneInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
	}


}

void hhl::net::TcpConnection::setTcpNoDelay(bool on)
{
	socket_->setTcpNoDelay(on);
}

void hhl::net::TcpConnection::startRead()
{
	loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, this));
	//channel_->enableReading();
}

void hhl::net::TcpConnection::stopRead()
{
	loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void hhl::net::TcpConnection::sendInLoop(const void * message, size_t len)
{
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;
	if (state_ == kDisconnected)
	{
		LOG_DEBUG << "disconnected, give up writing";
		return;
	}
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
	{
		nwrote = sockets::write(channel_->fd(), message, len);

		if (nwrote >= 0)
		{
			if (nwrote == 0 && writeCompleteCallback_)
			{
				//void (TcpConnection::*func)(void) = std::bind(writeCompleteCallback_,(TcpConnection)(*this));
				loop_->queneInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
			}
		}
		else
		{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
			{
				LOG_DEBUG << "TcpConnection::sendInLoop";
				if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
				{
					faultError = true;
				}
			}
		}
		assert(remaining <= len);
		if (!faultError && remaining > 0)
		{
			size_t oldLen = outputBuffer_.readableBytes();
			if (oldLen + remaining >= highWaterMark_
				&& oldLen < highWaterMark_
				&& highWaterMarkCallback_)
			{
				loop_->queneInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
			}
			outputBuffer_.append(static_cast<const char*>(message) + nwrote, remaining);
			if (!channel_->isWriting())
			{
				channel_->enableWriting();
			}
		}
	}

}

void hhl::net::TcpConnection::sendInLoop(const std::string & mesg)
{
	sendInLoop(mesg.data(), mesg.size());
}




void hhl::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
	
	LOG_DEBUG <<  " is "
		<< (conn->connected() ? "UP" : "DOWN");
	 //do not call conn->forceClose(), because some users want to register message callback only.
}

void hhl::net::defaultMessageCallback(const TcpConnectionPtr&,
	Buffer* buf,
	TimeStamp)
{
	buf->retrieveAll();
}

void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if (channel_->isWriting())
  {
	  ssize_t n = sockets::write(channel_->fd(),
		  outputBuffer_.peek(),
		  outputBuffer_.readableBytes());

	  if (n > 0)
	  {
		  outputBuffer_.retrieve(n);
		  if (outputBuffer_.readableBytes() == 0)
		  {
			  channel_->disableWriting();
			  if (writeCompleteCallback_)
			  {
				  loop_->queneInLoop(std::bind(writeCompleteCallback_,
					  shared_from_this()));
			  }
			  if (state_ == kDisconnecting)
			  {
					 shutdownInLoop();
			  }
		  }
	  }
	    else
	     {
	       LOG_DEBUG << "TcpConnection::handleWrite";
	        if (state_ == kDisconnecting)
	        {
	          shutdownInLoop();
	        }
	     }
  }
  else
  {
	    LOG_TRACE << "Connection fd = " << channel_->fd()
	               << " is down, no more writing";
  }
 
}

void TcpConnection::handleRead(TimeStamp receiveTime)
{
	loop_->assertInLoopThread();
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	if (n > 0)
	{
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	}
	else if (n == 0)
	{
		handleClose();
	}
	else
	{
		errno = savedErrno;
		LOG_DEBUG << "TcpConnection::handleRead";
		handleError();
	}
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	LOG_TRACE << "fd = " << channel_->fd() << " state = " << stateToString();
	assert(state_ == kConnected || state_ == kDisconnecting);
	// we don't close fd, leave it to dtor, so we can find leaks easily.
	setState(kDisconnected);
	channel_->disableAll();

	TcpConnectionPtr guardThis(shared_from_this());
	connectionCallback_(guardThis);

	// must be the last line
	closeCallback_(guardThis);
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR << "TcpConnection::handleError [" << name_
		<< "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

const char* TcpConnection::stateToString() const
{
	switch (state_)
	{
	case kDisconnected:
		return "kDisconnected";
	case kConnecting:
		return "kConnecting";
	case kConnected:
		return "kConnected";
	case kDisconnecting:
		return "kDisconnecting";
	default:
		return "unknown state";
	}
}

void hhl::net::TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if (!channel_->isWriting())
	{
		// we are not writing
		socket_->shutdownWrite();
	}
}

void hhl::net::TcpConnection::forceCloseInLoop()
{
	loop_->assertInLoopThread();
	if (state_ == kConnected || state_ == kDisconnecting)
	{
		// as if we received 0 byte in handleRead();
		handleClose();
	}
}

void hhl::net::TcpConnection::startReadInLoop()
{
	loop_->assertInLoopThread();
	if (!reading_ || !channel_->isReading())
	{
		channel_->enableReading();
		reading_ = true;
	}
}

void hhl::net::TcpConnection::stopReadInLoop()
{
	loop_->assertInLoopThread();
	if (reading_ || channel_->isReading())
	{
		channel_->disableReading();
		reading_ = false;
	}
}







