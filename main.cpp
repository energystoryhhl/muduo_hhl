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


int main()
{
	std::cout << "log test program" << std::endl;
	//LOG_DEBUG<<"123"<<"this is a test log"<<"2333";
	//string str = "logfile.log";

	char text[256] = "this is a log!\n";


	hhl::FileUtil::LogFile logfile("hhl", 1000, true);
	logfile.append(text, strlen(text));


	//sleep(2);

	return 0;
}


