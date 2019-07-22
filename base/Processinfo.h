#pragma once

#include <string>
#include "unistd.h"


using std::string;

namespace hhl
{
	namespace ProcessInfo 
	{
		string hostname();

		pid_t pid();
	}
}


