#ifndef UTILITY_CXX_HEADER_DETECTION_H
#define UTILITY_CXX_HEADER_DETECTION_H

#include <string>
#include <vector>

namespace utility
{
	std::vector<std::string> getCxxHeaderPaths(const std::string& compilerName);
}

#endif // UTILITY_CXX_HEADER_DETECTION_H
