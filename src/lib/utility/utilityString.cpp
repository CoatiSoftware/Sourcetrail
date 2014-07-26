#include "utility/utilityString.h"

namespace utility
{
	bool isPrefix(const std::string& prefix, const std::string& text)
	{
		typedef std::pair<std::string::const_iterator, std::string::const_iterator> ResType;
		ResType res = std::mismatch(prefix.begin(), prefix.end(), text.begin());

		return res.first == prefix.end();
	}
}
