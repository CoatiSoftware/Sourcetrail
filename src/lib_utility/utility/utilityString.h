#ifndef UTILITY_STRING_H
#define UTILITY_STRING_H

#include <algorithm>
#include <deque>
#include <sstream>
#include <string>
#include <vector>

namespace utility
{
std::string encodeToUtf8(const std::wstring& s);
std::wstring decodeFromUtf8(const std::string& s);

template <typename ContainerType>
ContainerType split(const std::string& str, const std::string& delimiter);

template <typename ContainerType>
ContainerType split(const std::wstring& str, const std::wstring& delimiter);

std::deque<std::string> split(const std::string& str, char delimiter);
std::deque<std::string> split(const std::string& str, const std::string& delimiter);
std::vector<std::string> splitToVector(const std::string& str, char delimiter);
std::vector<std::string> splitToVector(const std::string& str, const std::string& delimiter);
std::vector<std::wstring> splitToVector(const std::wstring& str, wchar_t delimiter);
std::vector<std::wstring> splitToVector(const std::wstring& str, const std::wstring& delimiter);

template <typename ContainerType>
std::string join(const ContainerType& list, const std::string& delimiter);

template <typename ContainerType>
std::wstring join(const ContainerType& list, const std::wstring& delimiter);

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
std::wstring substrBeforeLast(const std::wstring& str, wchar_t delimiter);
std::wstring substrAfterLast(const std::wstring& str, wchar_t delimiter);
std::string substrAfter(const std::string& str, char delimiter);
std::string substrAfter(const std::string& str, const std::string& delimiter);

template <typename StringType>
StringType substrBetween(
	const StringType& str, const StringType& delimiter1, const StringType& delimiter2);

template <typename StringType>
bool isPrefix(const StringType& prefix, const StringType& text);

template <typename StringType>
bool isPostfix(const StringType& postfix, const StringType& text);

std::string toUpperCase(const std::string& in);
std::wstring toUpperCase(const std::wstring& in);
std::string toLowerCase(const std::string& in);
std::wstring toLowerCase(const std::wstring& in);

template <typename StringType>
bool equalsCaseInsensitive(const std::string& a, const std::string& b);

std::string replace(std::string str, const std::string& from, const std::string& to);
std::wstring replace(std::wstring str, const std::wstring& from, const std::wstring& to);

std::string replaceBetween(
	const std::string& str, char startDelimiter, char endDelimiter, const std::string& to);
std::wstring replaceBetween(
	const std::wstring& str, wchar_t startDelimiter, wchar_t endDelimiter, const std::wstring& to);

std::string insertLineBreaksAtBlankSpaces(const std::string& s, size_t maxLineLength);
std::wstring breakSignature(std::wstring signature, size_t maxLineLength, size_t tabWidth);
std::wstring breakSignature(
	std::wstring returnPart,
	std::wstring namePart,
	std::wstring paramPart,
	size_t maxLineLength,
	size_t tabWidth);

std::string trim(const std::string& str);
std::wstring trim(const std::wstring& str);

enum ElideMode
{
	ELIDE_LEFT,
	ELIDE_MIDDLE,
	ELIDE_RIGHT
};

std::string elide(const std::string& str, ElideMode mode, size_t size);
std::wstring elide(const std::wstring& str, ElideMode mode, size_t size);

std::wstring convertWhiteSpacesToSingleSpaces(const std::wstring& str);

bool caseInsensitiveLess(const std::wstring& s1, const std::wstring& s2);

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
	} while (pos != std::string::npos);

	return c;
}

template <typename ContainerType>
ContainerType split(const std::wstring& str, const std::wstring& delimiter)
{
	size_t pos = 0;
	size_t oldPos = 0;
	ContainerType c;

	do
	{
		pos = str.find(delimiter, oldPos);
		c.push_back(str.substr(oldPos, pos - oldPos));
		oldPos = pos + delimiter.size();
	} while (pos != std::wstring::npos);

	return c;
}

template <typename ContainerType>
std::string join(const ContainerType& list, const std::string& delimiter)
{
	std::stringstream ss;
	bool first = true;
	for (const std::string& str: list)
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


template <typename ContainerType>
std::wstring join(const ContainerType& list, const std::wstring& delimiter)
{
	std::wstringstream ss;
	bool first = true;
	for (const std::wstring& str: list)
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


template <typename StringType>
StringType substrBetween(const StringType& str, const StringType& delimiter1, const StringType& delimiter2)
{
	size_t found_delimiter1 = str.find(delimiter1);
	found_delimiter1 += delimiter1.length();
	size_t found_delimiter2 = str.find(delimiter2, found_delimiter1);
	if (found_delimiter1 != str.npos && found_delimiter2 != str.npos)
	{
		return str.substr(found_delimiter1, found_delimiter2 - found_delimiter1);
	}
	return StringType();
}


template <typename StringType>
bool isPrefix(const StringType& prefix, const StringType& text)
{
	if (prefix.size() <= text.size())
	{
		std::pair<typename StringType::const_iterator, typename StringType::const_iterator> res =
			std::mismatch(prefix.begin(), prefix.end(), text.begin());

		return res.first == prefix.end();
	}
	return false;
}

template <typename StringType>
bool isPostfix(const StringType& postfix, const StringType& text)
{
	return text.size() >= postfix.size() && text.rfind(postfix) == (text.size() - postfix.size());
}

template <typename StringType>
bool equalsCaseInsensitive(const StringType& a, const StringType& b)
{
	if (a.size() == b.size())
	{
		return toLowerCase(a) == toLowerCase(b);
	}
	return false;
}
}	 // namespace utility

#endif	  // UTILITY_STRING_H
