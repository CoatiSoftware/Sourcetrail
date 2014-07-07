#include "utility/utilityString.h"

namespace utility
{
	std::vector<std::string> split(const std::string& str, char delimiter)
	{
		return split(str, std::string(1, delimiter));
	}

	std::vector<std::string> split(const std::string& str, const std::string& delimiter)
	{
		size_t pos = 0;
		size_t oldPos = 0;
		std::vector<std::string> vec;

		do
		{
			pos = str.find(delimiter, oldPos);
			vec.push_back(str.substr(oldPos, pos - oldPos));
			oldPos = pos + delimiter.size();
		}
		while (pos != std::string::npos);

		return vec;
	}

	bool isPrefix(const std::string& prefix, const std::string& text)
	{
		typedef std::pair<std::string::const_iterator, std::string::const_iterator> ResType;
		ResType res = std::mismatch(prefix.begin(), prefix.end(), text.begin());

		return res.first == prefix.end();
	}
}
