#ifndef UTILITY_Q_STRING_H
#define UTILITY_Q_STRING_H

#include <string>

namespace utility
{
	std::wstring decodeFromUtf8(std::string s);
	std::string encodeToUtf8(std::wstring s);
}

#endif // UTILITY_Q_STRING_H

