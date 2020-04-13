#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <string>

#include "../../lib/utility/ApplicationArchitectureType.h"
#include "../../lib/utility/file/FilePath.h"
#include "../../lib/utility/OsType.h"

namespace utility
{
std::pair<int, std::string> executeProcess(
	const std::string& command,
	const FilePath& workingDirectory = FilePath(),
	const int timeout = 30000);
std::string executeProcessUntilNoOutput(
	const std::string& command, const FilePath& workingDirectory, int waitTime = 10000);
int executeProcessAndGetExitCode(
	const std::wstring& commandPath,
	const std::vector<std::wstring>& commandArguments,
	const FilePath& workingDirectory = FilePath(),
	const int timeout = 30000,
	bool logProcessOutput = false,
	std::wstring* errorMessage = nullptr);

void killRunningProcesses();
int getIdealThreadCount();

OsType getOsType();
std::string getOsTypeString();
ApplicationArchitectureType getApplicationArchitectureType();
}	 // namespace utility

#endif	  // UTILITY_APP_H
