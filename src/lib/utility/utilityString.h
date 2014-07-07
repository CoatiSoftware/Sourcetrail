#ifndef UTILITY_STRING_H
#define UTILITY_STRING_H

#include <string>
#include <vector>

namespace utility
{
	std::vector<std::string> split(const std::string& str, char delimiter);
	std::vector<std::string> split(const std::string& str, const std::string& delimiter);

	bool isPrefix(const std::string& prefix, const std::string& text);
}

#endif // UTILITY_STRING_H
