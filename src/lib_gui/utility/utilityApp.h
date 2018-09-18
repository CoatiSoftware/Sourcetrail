#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <mutex>
#include <set>
#include <string>

#include "OsType.h"
#include "FilePath.h"

class License;

namespace utility
{
	std::string executeProcess(const std::string& command, const  FilePath& workingDirectory = FilePath(), const int timeout = 30000);
	std::string executeProcessUntilNoOutput(const std::string& command, const FilePath& workingDirectory, int waitTime = 10000);
	int executeProcessAndGetExitCode(
		const std::wstring& commandPath, 
		const std::vector<std::wstring>& commandArguments, 
		const FilePath& workingDirectory = FilePath(), 
		const int timeout = 30000
	);

	void killRunningProcesses();

	int getIdealThreadCount();

	OsType getOsType();

	bool saveLicense(const License* license);
}

#endif // UTILITY_APP_H

