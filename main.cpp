#include <iostream>
#include "Logging.h"
#include "FileUtil.h"
#include "LogFile.h"


#include <unistd.h>
#include <string.h>
#include <string>
#include <memory>

class test
{

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


int main()
{
	std::cout << "hello" << std::endl;
	//LOG_DEBUG<<"123"<<"this is a test log"<<"2333";
	//string str = "logfile.log";

	/*
	hhl::FileUtil::AppendFile log("hhl666.log");
	char buf[1024] = { 0 };

	while (strcmp(buf, "quite") != 0)
	{
		std::cout << "input some msg" << std::endl;
		std::cin >> buf;

		log.append(buf);
	}
	*/
	hhl::FileUtil::LogFile logfile("loghhl", 500, false);
	logfile.append("this is a test", 14);




	return 0;
}