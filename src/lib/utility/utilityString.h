#ifndef UTILITY_STRING_H
#define UTILITY_STRING_H

#include <deque>
#include <sstream>
#include <string>
#include <vector>

namespace utility
{
	template <typename ContainerType>
	ContainerType split(const std::string& str, const std::string& delimiter);

	std::deque<std::string> split(const std::string& str, char delimiter);
	std::deque<std::string> split(const std::string& str, const std::string& delimiter);
	std::vector<std::string> splitToVector(const std::string& str, char delimiter);
	std::vector<std::string> splitToVector(const std::string& str, const std::string& delimiter);

	template <typename ContainerType>
	std::string join(const ContainerType& list, const std::string& delimiter);

	std::string join(const std::deque<std::string>& list, char delimiter);
	std::string join(const std::deque<std::string>& list, const std::string& delimiter);
	std::string join(const std::vector<std::string>& list, char delimiter);
	std::string join(const std::vector<std::string>& list, const std::string& delimiter);

	std::deque<std::string> tokenize(const std::string& str, char delimiter);
	std::deque<std::string> tokenize(const std::string& str, const std::string& delimiter);
	std::deque<std::string> tokenize(const std::deque<std::string>& list, char delimiter);
	std::deque<std::string> tokenize(const std::deque<std::string>& list, const std::string& delimiter);

	std::string substrAfter(const std::string& str, char delimiter);

	bool isPrefix(const std::string& prefix, const std::string& text);

	bool equalsCaseInsensitive(const std::string& a, const std::string& b);


	template <typename ContainerType>
	ContainerType split(const std::string& str, const std::string& delimiter)
	{
		size_t pos = 0;
		size_t oldPos = 0;
		ContainerType c;

		do
		{
			pos = str.find(delimiter, oldPos);
			c.push_back(str.substr(oldPos, pos - oldPos));
			oldPos = pos + delimiter.size();
		}
		while (pos != std::string::npos);

		return c;
	}

	template <typename ContainerType>
	std::string join(const ContainerType& list, const std::string& delimiter)
	{
		std::stringstream ss;
		bool first = true;
		for (const std::string& str : list)
		{
			if (!first)
			{
				ss << delimiter;
			}
			first = false;

			ss << str;
		}
		return ss.str();
	}
}

#endif // UTILITY_STRING_H
