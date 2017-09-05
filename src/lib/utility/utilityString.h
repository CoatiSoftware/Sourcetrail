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

	std::string substrBeforeFirst(const std::string& str, char delimiter);
	std::string substrBeforeFirst(const std::string& str, const std::string& delimiter);
	std::string substrBeforeLast(const std::string& str, char delimiter);
	std::string substrAfter(const std::string& str, char delimiter);
	std::string substrAfter(const std::string& str, const std::string& delimiter);

	std::string substrBetween(const std::string& str, const std::string& delimiter1, const std::string& delimiter2);

	bool isPrefix(const std::string& prefix, const std::string& text);
	bool isPostfix(const std::string& postfix, const std::string& text);

	std::string toUpperCase(const std::string& in);
	std::string toLowerCase(const std::string& in);
	bool equalsCaseInsensitive(const std::string& a, const std::string& b);

	std::string replace(std::string str, const std::string& from, const std::string& to);
	std::string replaceBetween(const std::string& str, char startDelimiter, char endDelimiter, const std::string& to);

	std::string insertLineBreaksAtBlankSpaces(const std::string& s, size_t maxLineLength);
	std::string breakSignature(
		std::string returnPart, std::string namePart, std::string paramPart,
		size_t maxLineLength, size_t tabWidth);

	std::string trim(const std::string &str);

	enum ElideMode
	{
		ELIDE_LEFT,
		ELIDE_MIDDLE,
		ELIDE_RIGHT
	};

	std::string elide(const std::string& str, ElideMode mode, size_t size);

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
