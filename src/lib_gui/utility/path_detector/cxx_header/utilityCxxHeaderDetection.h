#ifndef UTILITY_CXX_HEADER_DETECTION_H
#define UTILITY_CXX_HEADER_DETECTION_H

#include <string>
#include <vector>

#include "ApplicationArchitectureType.h"
#include "FilePath.h"

namespace utility
{
std::vector<std::wstring> getCxxHeaderPaths(const std::string& compilerName);

std::vector<FilePath> getWindowsSdkHeaderSearchPaths(ApplicationArchitectureType architectureType);
FilePath getWindowsSdkRootPathUsingRegistry(
	ApplicationArchitectureType architectureType, const std::string& sdkVersion);
}	 // namespace utility

#endif	  // UTILITY_CXX_HEADER_DETECTION_H
