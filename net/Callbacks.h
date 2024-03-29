#pragma once

#include <functional>
#include <memory>

#include "net/Buffer.h"
#include "base/TimeStamp.h"
using namespace hhl::base;

namespace hhl
{
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;

	template<typename T>
	inline T* get_pointer(const std::shared_ptr<T>& ptr)
	{
		return ptr.get();
	}


	template<typename T>
	inline T* get_pointer(const std::unique_ptr<T>& ptr)
	{
		return ptr.get();
	}

	namespace net
	{
		class TcpConnection;
		class Buffer;

		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void()> TimerCallback;
		typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
		typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
		typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
		typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

		typedef std::function<void(const TcpConnectionPtr&,
			Buffer*,
			TimeStamp)> MessageCallback;

		void defaultConnectionCallback(const TcpConnectionPtr& conn);
		void defaultMessageCallback(const TcpConnectionPtr& conn,
			Buffer* buffer,
			TimeStamp receiveTime);



	}


}