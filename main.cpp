#include <iostream>
#include "Logging.h"
#include "FileUtil.h"
#include "LogFile.h"

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