#include "net/Connector.h"
#include "Logging.h"
#include "net/EventLoop.h"
#include "net/SocketOps.h"
#include "net/Channel.h"

using namespace hhl;
using namespace hhl::net;

 const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop * loop, const InetAddress & serverAddr)
	:loop_(loop),
	serverAddr_(serverAddr),
	connect_(false),
	state_(kDisconnected),
	retryDelayMs_(kInitRetryDelayMs)
{
	LOG_DEBUG << "connector[" << this << "]";
}

void Connector::start()
{
	connect_ = true;
	loop_->runInLoop(std::bind(&Connector::startInLoop, this));

}

void hhl::net::Connector::startInLoop()
{
	loop_->assertInLoopThread();
	//assert(state_ == KDisconnected);
	if (connect_)
	{
		connect();
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}

}

void hhl::net::Connector::connect()
{
	int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
	int savedErrno = (ret == 0) ? 0 : errno;
	switch (savedErrno)
	{
	case 0:
	case EINPROGRESS:		//�������ڷ�æ
	case EINTR:				//sys call ���ж�
	case EISCONN:			//�Ѿ����ӵ����׽���
		connecting(sockfd);
		break;

	case EAGAIN:			//û���㹻���еı��ض˿�
	case EADDRINUSE:		//���ص�ַ����ʹ��״̬
	case EADDRNOTAVAIL:
	case ECONNREFUSED:		//Զ�̵�ַ��û�д��ڼ���״̬
	case ENETUNREACH:		//���粻�ɵ���
		retry(sockfd);
		break;

	case EACCES:			//�û���ͼ���׽��ֹ㲥��־û�����õ���������ӹ㲥��ַ�����ڷ���ǽ���Ե�������ʧ�ܡ�
	case EPERM:
	case EAFNOSUPPORT:		//����serv_add�еĵ�ַ�ǺϷ���ַ
	case EALREADY:			//�׽���Ϊ�������׽��֣�����ԭ������������δ��
	case EBADF:				//�Ƿ����ļ�������
	case EFAULT:			//ָ���׽��ֽṹ��ĵ�ַ�Ƿ�
	case ENOTSOCK:			//�ļ������������׽������
		LOG_DEBUG << "connect error in Connector::startInLoop " << savedErrno;
		sockets::close(sockfd);
		break;

	default:
		LOG_DEBUG << "Unexpected error in Connector::startInLoop " << savedErrno;
		sockets::close(sockfd);
		// connectErrorCallback_();
		break;
	}
}

void hhl::net::Connector::connecting(int sockfd)
{
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	channel_->setWriteCallback(
		std::bind(&Connector::handleWrite, this)); // FIXME: unsafe
	channel_->setErrorCallback(
		std::bind(&Connector::handleError, this)); // FIXME: unsafe

	// channel_->tie(shared_from_this()); is not working,
	// as channel_ is not managed by shared_ptr
	channel_->enableWriting();
}

void hhl::net::Connector::handleWrite()
{
	LOG_TRACE << "Connector::handleWrite " << state_;

	if (state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if (err)
		{
			LOG_DEBUG << "Connector::handleWrite - SO_ERROR = "
				<< err << " " << strerror_tl(err);
			retry(sockfd);
		}
		else if (sockets::isSelfConnect(sockfd))
		{
			LOG_DEBUG << "Connector::handleWrite - Self connect";
			retry(sockfd);
		}
		else
		{
			setState(kConnected);
			if (connect_)
			{
				newConnectionCallback_(sockfd);
			}
			else
			{
				sockets::close(sockfd);
			}
		}
	}
	else
	{
		// what happened?
		assert(state_ == kDisconnected);
	}

}

void hhl::net::Connector::handleError()
{
	LOG_ERROR << "Connector::handleError state=" << state_;
	if (state_ == kConnecting)
	{
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
		retry(sockfd);
	}
}

void hhl::net::Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(kDisconnected);
	if (connect_)
	{
		LOG_DEBUG << "Connector::retry - Retry connecting to " << serverAddr_.tpIPPort()
			<< " in " << retryDelayMs_ << " milliseconds. ";
		loop_->runAfter(retryDelayMs_ / 1000.0,
			std::bind(&Connector::startInLoop, shared_from_this()));
		retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
	}
	else
	{
		LOG_DEBUG << "do not connect";
	}

}

Connector::~Connector()
{
	LOG_DEBUG << "dtor[" << this << "]";
	assert(!channel_);
}

int hhl::net::Connector::removeAndResetChannel()
{
	channel_->disableAll();
	channel_->remove();
	int sockfd = channel_->fd();
	// Can't reset channel_ here, because we are inside Channel::handleEvent
	loop_->queneInLoop(std::bind(&Connector::resetChannel, this)); // FIXME: unsafe
	return sockfd;
}

void Connector::resetChannel()
{
	channel_.reset();
}

void hhl::net::Connector::stopInLoop()
{
	loop_->assertInLoopThread();
	if (state_ == kConnecting)
	{
		setState(kDisconnected);
		int sockfd = removeAndResetChannel();
		retry(sockfd);
	}
}

void Connector::restart()
{
	loop_->assertInLoopThread();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	connect_ = true;
	startInLoop();
}

void Connector::stop()
{
	connect_ = false;
	loop_->queneInLoop(std::bind(&Connector::stopInLoop, this)); // FIXME: unsafe
	// FIXME: cancel timer
}