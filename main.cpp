#include <iostream>
#include "Logging.h"
#include "FileUtil.h"
#include "LogFile.h"


#include <unistd.h>
#include <string.h>
#include <string>
#include <memory>

#include <map>
#include <string>
#include <functional>
#include "thread.h"

#include "countDownLatch.h"
#include "AsyncLogging.h"
#include "base/TimeStamp.h"

class test
{
public:
	class inner_class {
	public:
		inner_class() {
			std::cout << "inner_class struct called!" << std::endl;
		}
		~inner_class() {
			std::cout << "inner_class destruct called!" << std::endl;
		}
	};

public:
	test() :
	p_string(new inner_class)
	{
		std::cout << "struct called!" << std::endl;
	
	}

	~test()
	{
		std::cout << "destruct called!" << std::endl;
	}

	void show()
	{
		//std::cout << *p_string << std::endl;
	}

public:
	std::unique_ptr<inner_class> p_string;

};

class stock
{
private:
	/* data */
	
public:
	stock(/* args */std::string key)
	:
	name_(key)
	{
		std::cout<<name_<<"stock construct!"<<std::endl;
	}
	~stock();

	std::string name_;
};


stock::~stock()
{
	std::cout<<name_<<"stock deconstruct!"<<std::endl;
}


class Factory
{
private:
	/* data */
	std::map<std::string, std::weak_ptr<stock>> stocks_;
	hhl::MutexLock mutex_;

	void deleteStock(stock *stock)
	{
		if(stock)
		{
			hhl::MutexLockGuard lock(mutex_);
			stocks_.erase(stock->name_);
		}
		delete stock;
		std::cout<<"after delete factory num: "<<stocks_.size()<<std::endl;
	}

public:
	Factory(/* args */)
	:
	stocks_(),
	mutex_()
	{

	}
	;
	~Factory()
	{}
	
	std::shared_ptr<stock> get(std::string key)
	{
		std::shared_ptr<stock> pStock;
		hhl::MutexLockGuard lock(mutex_);
		std::weak_ptr<stock>& wpStock = stocks_[key];
		pStock = wpStock.lock();
		if(pStock == NULL)
		{
			pStock.reset(new stock(key)
							, std::bind(&Factory::deleteStock, this, std::placeholders::_1));
		}
		std::cout<<"factory num: "<<stocks_.size()<<std::endl;
		return pStock;
	}

	void insert(stock s)
	{
		hhl::MutexLockGuard lock(mutex_);
		stocks_[s.name_] = std::make_shared<stock>(s);
	}

};

void fun1(int v1,int v2)
{
	std::cout<<"v1 is:"<<v1<<std::endl;
	std::cout<<"v2 is:"<<v2<<std::endl;
}
	CountDownLatch latch(2);
void * func(void *)
{
	std::cout<<"func start !"<<std::endl;
	latch.wait();
	std::cout<<"func done !"<<std::endl;
}

void functest()
{
	std::cout<<"this is test thread!\n"<<std::endl;
	sleep(1);
}


hhl::AsyncLogging * g_asynclog = new hhl::AsyncLogging(string("hhl_log"),7000);

		void AsyncLogOutPut(const char* log,int len)
        {
            if(g_asynclog != NULL)
            {
                g_asynclog->append(log, len);
            }
        }


using namespace hhl;

int main()
{
	/* 
	std::cout << "log test program" << std::endl;

	char text[256] = "this is a log!\n";

	hhl::FileUtil::LogFile logfile("hhl", 1000, true);
	logfile.append(text, strlen(text));
	logfile.flush();
	*/ 

	// hhl::thread thread1(std::bind(functest),string("hhl_thread"));

	// thread1.start();
	// hhl::base::TimeStamp tm1(hhl::base::TimeStamp().now());
	// sleep(2);
	// hhl::base::TimeStamp tm2(hhl::base::TimeStamp().now());

	// if(tm1<tm2) 
	// std::cout<<"ok!"  ;



	std::cout << hhl::base::TimeStamp().now().toFormatString()<<std::endl;
	exit(0);
	hhl::Logger::setOutPutFunc(::AsyncLogOutPut);


	g_asynclog->start();


	//LOG_DEBUG<<"this is a test log"<<"this is also a test!\n";
	

	long int i = 0;
	char buffer[1024] = {0};
	while(1)
	{
		// std::cout<<i<<std::endl;
		// if(i!=0 && (i%30 == 0))
		// {
		// 	sleep(5);
		// }
		// LOG_DEBUG<<i<<"his is a test log"<<"this is also a test!";
		// i++;
		std::cin>>buffer;
		LOG_DEBUG<<buffer;
		memset(buffer,0,1024);
	};

	return 0;
}


