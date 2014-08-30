#ifndef UTILITY_STRING_H
#define UTILITY_STRING_H

#include <string>
#include <vector>

namespace utility
{
	template<typename ContainerType>
	ContainerType split(const std::string& str, char delimiter);

	template<typename ContainerType>
	ContainerType split(const std::string& str, const std::string& delimiter);

	template<typename ContainerType>
	ContainerType tokenize(const std::string& str, char delimiter);

	template<typename ContainerType>
	ContainerType tokenize(const std::string& str, const std::string& delimiter);

	template<typename ContainerType>
	ContainerType tokenize(const ContainerType& list, char delimiter);

	template<typename ContainerType>
	ContainerType tokenize(const ContainerType& list, const std::string& delimiter);

	std::string substrAfter(const std::string& str, char delimiter);

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

template<typename ContainerType>
ContainerType utility::tokenize(const std::string& str, char delimiter)
{
	return tokenize<ContainerType>(str, std::string(1, delimiter));
}

template<typename ContainerType>
ContainerType utility::tokenize(const std::string& str, const std::string& delimiter)
{
	size_t pos = 0;
	size_t oldPos = 0;
	ContainerType c;

	do
	{
		pos = str.find(delimiter, oldPos);

		if (pos != oldPos)
		{
			c.push_back(str.substr(oldPos, pos - oldPos));
		}

		if (pos != std::string::npos)
		{
			c.push_back(str.substr(pos, delimiter.size()));
		}

		oldPos = pos + delimiter.size();
	}
	while (pos != std::string::npos && oldPos < str.size());

	return c;
}

template<typename ContainerType>
ContainerType utility::tokenize(const ContainerType& list, char delimiter)
{
	return tokenize<ContainerType>(list, std::string(1, delimiter));
}

template<typename ContainerType>
ContainerType utility::tokenize(const ContainerType& list, const std::string& delimiter)
{
	ContainerType c;

	for (std::string str : list)
	{
		ContainerType c2 = tokenize<ContainerType>(str, delimiter);
		c.insert(c.end(), c2.begin(), c2.end());
	}

	return c;
}

#endif // UTILITY_STRING_H
