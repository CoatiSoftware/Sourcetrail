#ifndef UTILITY_STRING_H
#define UTILITY_STRING_H

#include <deque>
#include <sstream>
#include <string>

namespace utility
{
	std::deque<std::string> split(const std::string& str, char delimiter);
	std::deque<std::string> split(const std::string& str, const std::string& delimiter);

	std::string join(const std::deque<std::string>& list, char delimiter);
	std::string join(const std::deque<std::string>& list, const std::string& delimiter);

	std::deque<std::string> tokenize(const std::string& str, char delimiter);
	std::deque<std::string> tokenize(const std::string& str, const std::string& delimiter);
	std::deque<std::string> tokenize(const std::deque<std::string>& list, char delimiter);
	std::deque<std::string> tokenize(const std::deque<std::string>& list, const std::string& delimiter);

	std::string substrAfter(const std::string& str, char delimiter);

	bool isPrefix(const std::string& prefix, const std::string& text);

	bool equalsCaseInsensitive(const std::string& a, const std::string& b);
}

#endif // UTILITY_STRING_H
