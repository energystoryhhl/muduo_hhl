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

#include "countDownLatch.h"

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

int main()
{
	/* 
	std::cout << "log test program" << std::endl;

	char text[256] = "this is a log!\n";

	hhl::FileUtil::LogFile logfile("hhl", 1000, true);
	logfile.append(text, strlen(text));
	logfile.flush();
	*/ 

	//sleep(2);
	pthread_t pid;

	pthread_create(&pid, NULL, func,NULL);

	sleep(2);

	latch.countDown();

	sleep(2);

	latch.countDown();

	while(1)
	{
		sleep(2);
	};

	return 0;
}


