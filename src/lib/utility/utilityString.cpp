#include "utility/utilityString.h"

namespace utility
{
	std::deque<std::string> split(const std::string& str, char delimiter)
	{
		return split(str, std::string(1, delimiter));
	}

	std::deque<std::string> split(const std::string& str, const std::string& delimiter)
	{
		size_t pos = 0;
		size_t oldPos = 0;
		std::deque<std::string> c;

		do
		{
			pos = str.find(delimiter, oldPos);
			c.push_back(str.substr(oldPos, pos - oldPos));
			oldPos = pos + delimiter.size();
		}
		while (pos != std::string::npos);

		return c;
	}

	std::string join(const std::deque<std::string>& list, char delimiter)
	{
		return join(list, std::string(1, delimiter));
	}

	std::string join(const std::deque<std::string>& list, const std::string& delimiter)
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

		for (std::string str : list)
		{
			if (str.size())
			{
				std::deque<std::string> c2 = tokenize(str, delimiter);
				c.insert(c.end(), c2.begin(), c2.end());
			}
		}

		return c;
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

	bool isPrefix(const std::string& prefix, const std::string& text)
	{
		typedef std::pair<std::string::const_iterator, std::string::const_iterator> ResType;
		ResType res = std::mismatch(prefix.begin(), prefix.end(), text.begin());

		return res.first == prefix.end();
	}
}
