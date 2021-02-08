#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <string>

#include "ApplicationArchitectureType.h"
#include "FilePath.h"
#include "OsType.h"

namespace utility
{
struct ProcessOutput
{
	ProcessOutput(std::wstring output, int exitCode);
	std::wstring output;
	int exitCode;
};

std::wstring searchPath(std::wstring bin);
ProcessOutput executeProcessBoost(
	const std::wstring& command,
	const FilePath& workingDirectory = FilePath(),
	const int timeout = 30000);
ProcessOutput executeProcessBoost(
	const std::wstring& command,
	const std::vector<std::wstring>& arguments,
	const FilePath& workingDirectory = FilePath(),
	const int timeout = 30000);
ProcessOutput executeProcess(
	const std::wstring& commandPath,
	const std::vector<std::wstring>& commandArguments,
	const FilePath& workingDirectory = FilePath(),
	const int timeout = 30000);
std::string executeProcessUntilNoOutput(
	const std::wstring& commandPath,
	const std::vector<std::wstring>& commandArguments,
	const FilePath& workingDirectory,
	int waitTime = 10000);
int executeProcessAndGetExitCode(
	const std::wstring& commandPath,
	const std::vector<std::wstring>& commandArguments,
	const FilePath& workingDirectory = FilePath(),
	const int timeout = 30000,
	bool logProcessOutput = false,
	std::wstring* errorMessage = nullptr);

void killRunningProcesses();
int getIdealThreadCount();

constexpr OsType getOsType()
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	return OS_WINDOWS;
#elif defined(__APPLE__)
	return OS_MAC;
#elif defined(__linux) || defined(__linux__) || defined(linux)
	return OS_LINUX;
#else
	return OS_UNKNOWN;
#endif
}

std::string getOsTypeString();

constexpr ApplicationArchitectureType getApplicationArchitectureType()
{
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64) ||             \
	defined(WIN64)
	return APPLICATION_ARCHITECTURE_X86_64;
#else
	return APPLICATION_ARCHITECTURE_X86_32;
#endif
	return APPLICATION_ARCHITECTURE_UNKNOWN;
}
}	 // namespace utility

#endif	  // UTILITY_APP_H
