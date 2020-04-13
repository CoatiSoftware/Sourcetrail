#ifndef UTILITY_CXX_HEADER_DETECTION_H
#define UTILITY_CXX_HEADER_DETECTION_H

#include <string>
#include <vector>

#include "../../../../lib/utility/ApplicationArchitectureType.h"
#include "../../../../lib/utility/file/FilePath.h"

namespace utility
{
std::vector<std::string> getCxxHeaderPaths(const std::string& compilerName);

std::vector<FilePath> getWindowsSdkHeaderSearchPaths(ApplicationArchitectureType architectureType);
FilePath getWindowsSdkRootPathUsingRegistry(
	ApplicationArchitectureType architectureType, const std::string& sdkVersion);
}	 // namespace utility

#endif	  // UTILITY_CXX_HEADER_DETECTION_H
