#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <mutex>
#include <set>
#include <string>

#include "utility/OsType.h"

class License;

namespace utility
{
	std::string executeProcess(const std::string& command, const std::string& workingDirectory = "", const int timeout = 30000);
	std::string executeProcessUntilNoOutput(const std::string& command, const std::string& workingDirectory, int waitTime = 10000);
	int executeProcessAndGetExitCode(const std::string& command, const std::string& workingDirectory = "", const int timeout = 30000);

	void killRunningProcesses();

	int getIdealThreadCount();

	OsType getOsType();

	bool saveLicense(const License* license);
}

#endif // UTILITY_APP_H

