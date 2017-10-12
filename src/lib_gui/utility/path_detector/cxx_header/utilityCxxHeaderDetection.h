#ifndef UTILITY_CXX_HEADER_DETECTION_H
#define UTILITY_CXX_HEADER_DETECTION_H

#include <string>
#include <vector>

#include "utility/file/FilePath.h"
#include "utility/ApplicationArchitectureType.h"

namespace utility
{
	std::vector<std::string> getCxxHeaderPaths(const std::string& compilerName);

	std::vector<FilePath> getWindowsSdkHeaderSearchPaths(ApplicationArchitectureType architectureType);
	FilePath getWindowsSdkRootPathUsingRegistry(ApplicationArchitectureType architectureType, const std::string& sdkVersion);
}

#endif // UTILITY_CXX_HEADER_DETECTION_H
