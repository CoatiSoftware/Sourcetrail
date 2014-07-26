#ifndef UTILITY_STRING_H
#define UTILITY_STRING_H

#include <string>
#include <vector>

namespace utility
{
	template<typename ContainerType = std::vector<std::string>>
	ContainerType split(const std::string& str, char delimiter);

	template<typename ContainerType = std::vector<std::string>>
	ContainerType split(const std::string& str, const std::string& delimiter);

	bool isPrefix(const std::string& prefix, const std::string& text);
}

template<typename ContainerType>
ContainerType utility::split(const std::string& str, char delimiter)
{
	return split<ContainerType>(str, std::string(1, delimiter));
}

template<typename ContainerType>
ContainerType utility::split(const std::string& str, const std::string& delimiter)
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

#endif // UTILITY_STRING_H
