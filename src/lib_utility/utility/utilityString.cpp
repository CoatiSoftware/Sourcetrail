#include "utility/utilityString.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <string>

#include <boost/locale/encoding_utf.hpp>

namespace
{
	template <typename StringType>
	StringType doReplace(StringType str, const StringType& from, const StringType& to)
	{
		size_t pos = 0;

		if (from.size() == 0)
		{
			return str;
		}

		while ((pos = str.find(from, pos)) != std::string::npos)
		{
			str.replace(pos, from.length(), to);
			pos += to.length();
		}

		return str;
	}

	template <typename StringType>
	StringType doReplaceBetween(const StringType& str, typename StringType::value_type startDelimiter, typename StringType::value_type endDelimiter, const StringType& to)
	{
		size_t startPos = str.find(startDelimiter);
		if (startPos == StringType::npos)
		{
			return str;
		}

		size_t depth = 1;

		for (size_t pos = startPos + 1; pos < str.size(); pos++)
		{
			if (str[pos] == endDelimiter && depth)
			{
				depth--;

				if (depth == 0)
				{
					StringType end = doReplaceBetween<StringType>(str.substr(pos + 1), startDelimiter, endDelimiter, to);
					return str.substr(0, startPos) + startDelimiter + to + endDelimiter + end;
				}
			}

			if (str[pos] == startDelimiter)
			{
				depth++;
			}
		}

		return str;
	}
}

namespace utility
{
	std::string encodeToUtf8(const std::wstring& s)
	{
		return boost::locale::conv::utf_to_utf<char>(s.c_str(), s.c_str() + s.size());
	}

	std::wstring decodeFromUtf8(const std::string& s)
	{
		return boost::locale::conv::utf_to_utf<wchar_t>(s.c_str(), s.c_str() + s.size());
	}

	std::deque<std::string> split(const std::string& str, char delimiter)
	{
		return split<std::deque<std::string>>(str, std::string(1, delimiter));
	}

	std::deque<std::string> split(const std::string& str, const std::string& delimiter)
	{
		return split<std::deque<std::string>>(str, delimiter);
	}

	std::vector<std::string> splitToVector(const std::string& str, char delimiter)
	{
		return split<std::vector<std::string>>(str, std::string(1, delimiter));
	}

	std::vector<std::string> splitToVector(const std::string& str, const std::string& delimiter)
	{
		return split<std::vector<std::string>>(str, delimiter);
	}

	std::vector<std::wstring> splitToVector(const std::wstring& str, wchar_t delimiter)
	{
		return split<std::vector<std::wstring>>(str, std::wstring(1, delimiter));
	}

	std::vector<std::wstring> splitToVector(const std::wstring& str, const std::wstring& delimiter)
	{
		return split<std::vector<std::wstring>>(str, delimiter);
	}

	std::string join(const std::deque<std::string>& list, char delimiter)
	{
		return join<std::deque<std::string> >(list, std::string(1, delimiter));
	}

	std::string join(const std::deque<std::string>& list, const std::string& delimiter)
	{
		return join<std::deque<std::string> >(list, delimiter);
	}

	std::string join(const std::vector<std::string>& list, char delimiter)
	{
		return join<std::vector<std::string> >(list, std::string(1, delimiter));
	}

	std::string join(const std::vector<std::string>& list, const std::string& delimiter)
	{
		return join<std::vector<std::string> >(list, delimiter);
	}

	std::deque<std::string> tokenize(const std::string& str, char delimiter)
	{
		return tokenize(str, std::string(1, delimiter));
	}

	std::deque<std::string> tokenize(const std::string& str, const std::string& delimiter)
	{
		size_t pos = 0;
		size_t oldPos = 0;
		std::deque<std::string> c;

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

	std::deque<std::string> tokenize(const std::deque<std::string>& list, char delimiter)
	{
		return tokenize(list, std::string(1, delimiter));
	}

	std::deque<std::string> tokenize(const std::deque<std::string>& list, const std::string& delimiter)
	{
		std::deque<std::string> c;

		for (const std::string& str : list)
		{
			if (str.size())
			{
				std::deque<std::string> c2 = tokenize(str, delimiter);
				c.insert(c.end(), c2.begin(), c2.end());
			}
		}

		return c;
	}

	std::string substrBeforeFirst(const std::string& str, char delimiter)
	{
		size_t pos = str.find(delimiter);
		if (pos != std::string::npos)
		{
			return str.substr(0, pos);
		}
		return str;
	}

	std::string substrBeforeFirst(const std::string& str, const std::string& delimiter)
	{
		size_t pos = str.find(delimiter);
		if (pos != std::string::npos)
		{
			return str.substr(0, pos);
		}
		return str;
	}

	std::string substrBeforeLast(const std::string& str, char delimiter)
	{
		size_t pos = str.rfind(delimiter);
		if (pos != std::string::npos)
		{
			return str.substr(0, pos);
		}
		return str;
	}

	std::wstring substrBeforeLast(const std::wstring& str, wchar_t delimiter)
	{
		size_t pos = str.rfind(delimiter);
		if (pos != std::wstring::npos)
		{
			return str.substr(0, pos);
		}
		return str;
	}

	std::string substrAfter(const std::string& str, char delimiter)
	{
		size_t pos = str.find(delimiter);
		if (pos != std::string::npos)
		{
			return str.substr(pos + 1, str.size());
		}
		return str;
	}

	std::string substrAfter(const std::string& str, const std::string& delimiter)
	{
		size_t pos = str.find(delimiter);
		if (pos != std::string::npos)
		{
			return str.substr(pos + delimiter.size(), str.size());
		}
		return str;
	}

	std::string toUpperCase(const std::string& in)
	{
		std::string out;
		std::transform(in.begin(), in.end(), std::back_inserter(out), toupper);
		return out;
	}

	std::string toLowerCase(const std::string& in)
	{
		std::string out;
		std::transform(in.begin(), in.end(), std::back_inserter(out), tolower);
		return out;
	}

	std::wstring toLowerCase(const std::wstring& in)
	{
		std::wstring out;
		std::transform(in.begin(), in.end(), std::back_inserter(out), tolower);
		return out;
	}

	std::string replace(std::string str, const std::string& from, const std::string& to)
	{
		return doReplace(str, from, to);
	}

	std::wstring replace(std::wstring str, const std::wstring& from, const std::wstring& to)
	{
		return doReplace(str, from, to);
	}

	std::string replaceBetween(const std::string& str, char startDelimiter, char endDelimiter, const std::string& to)
	{
		return doReplaceBetween<std::string>(str, startDelimiter, endDelimiter, to);
	}

	std::wstring replaceBetween(const std::wstring& str, wchar_t startDelimiter, wchar_t endDelimiter, const std::wstring& to)
	{
		return doReplaceBetween<std::wstring>(str, startDelimiter, endDelimiter, to);
	}
	
	std::string insertLineBreaksAtBlankSpaces(const std::string& s, size_t maxLineLength)
	{
		const std::vector<std::string> atoms = splitToVector(s, " ");

		std::string ret = "";
		std::string currentLine = "";
		for (const std::string& atom: atoms)
		{
			if (currentLine.size() + 1 + atom.size() <= maxLineLength)
			{
				currentLine += " " + atom;
			}
			else
			{
				if (!ret.empty())
				{
					ret += "\n";
				}

				if (currentLine.empty())
				{
					ret += atom;
				}
				else
				{
					ret += currentLine;
					currentLine = atom;
				}
			}
		}
		if (!currentLine.empty())
		{
			if (!ret.empty())
			{
				ret += "\n";
			}
			ret += currentLine;
		}
		return ret;
	}

	std::string breakSignature(
		std::string returnPart, std::string namePart, std::string paramPart,
		size_t maxLineLength, size_t tabWidth)
	{
		namePart = ' ' + namePart;

		size_t totalSize = returnPart.size() + namePart.size() + paramPart.size();
		if (totalSize <= maxLineLength)
		{
			return returnPart + namePart + paramPart;
		}

		if (paramPart.size())
		{
			namePart += paramPart[0];
			paramPart.erase(0, 1);
		}

		size_t parenPos = paramPart.rfind(')');
		std::string endPart;
		if (parenPos == 0)
		{
			namePart += paramPart;
			paramPart = "";
		}
		else if (parenPos != std::string::npos)
		{
			endPart = paramPart.substr(parenPos);
			paramPart = paramPart.substr(0, parenPos);
		}

		if (paramPart.size() && paramPart.size() + tabWidth - endPart.size() > maxLineLength)
		{
			std::vector<std::string> paramLines;
			while (true)
			{
				size_t parenCount = 0;
				bool split = false;
				for (size_t i = 0; i < paramPart.size(); i++)
				{
					char c = paramPart[i];
					if (parenCount == 0 && c == ',')
					{
						paramLines.push_back(paramPart.substr(0, i + 1));
						paramPart = paramPart.substr(i + 2);
						split = true;
						break;
					}
					else if (c == '<' || c == '(')
					{
						parenCount++;
					}
					else if (c == '>' || c == ')')
					{
						parenCount--;
					}
				}

				if (!split)
				{
					paramLines.push_back(paramPart);
					break;
				}
			}

			paramPart = "";
			for (const std::string& str : paramLines)
			{
				paramPart += "\n\t" + str;
				size_t length = tabWidth + str.size();
				maxLineLength = std::max(length, maxLineLength);
			}
		}
		else if (paramPart.size())
		{
			paramPart = "\n\t" + paramPart;
		}

		if (returnPart.size() + namePart.size() <= maxLineLength)
		{
			namePart = returnPart + namePart;
			returnPart = "";
		}

		std::string sig;

		if (returnPart.size())
		{
			sig += returnPart + '\n';
		}

		sig += namePart;

		if (paramPart.size())
		{
			sig += paramPart;
		}

		if (endPart.size())
		{
			sig += '\n' + endPart;
		}

		return sig;
	}

	std::string trim(const std::string &str)
	{
		auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) { return std::isspace(c); });
		auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) { return std::isspace(c); }).base();
		return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
	}

	std::wstring trim(const std::wstring &str)
	{
		auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) { return std::isspace(c); });
		auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) { return std::isspace(c); }).base();
		return (wsback <= wsfront ? std::wstring() : std::wstring(wsfront, wsback));
	}

	std::string elide(const std::string& str, ElideMode mode, size_t size)
	{
		if (str.size() <= size || str.size() <= 3)
		{
			return str;
		}

		switch (mode)
		{
		case ELIDE_LEFT:
			return "..." + str.substr(str.size() - size - 3, str.size());
		case ELIDE_MIDDLE:
			return str.substr(0, size / 2 - 1) + "..." + str.substr(str.size() - (size / 2 - 2), str.size());
		case ELIDE_RIGHT:
			return str.substr(0, size - 3) + "...";
		}
	}

	std::wstring elide(const std::wstring& str, ElideMode mode, size_t size)
	{
		if (str.size() <= size || str.size() <= 3)
		{
			return str;
		}

		switch (mode)
		{
		case ELIDE_LEFT:
			return L"..." + str.substr(str.size() - size - 3, str.size());
		case ELIDE_MIDDLE:
			return str.substr(0, size / 2 - 1) + L"..." + str.substr(str.size() - (size / 2 - 2), str.size());
		case ELIDE_RIGHT:
			return str.substr(0, size - 3) + L"...";
		}
	}
}
