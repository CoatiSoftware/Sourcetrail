#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <mutex>
#include <set>
#include <string>

#include "utility/ApplicationArchitectureType.h"

namespace utility
{
	std::string executeProcess(const std::string& command, const std::string& workingDirectory = "", int timeout = 30000);
	int executeProcessAndGetExitCode(const std::string& command, const std::string& workingDirectory = "", int timeout = 30000);

	void killRunningProcesses();

	int getIdealThreadCount();
}

#endif // UTILITY_APP_H

