#ifndef PARSE_VARIABLE_H
#define PARSE_VARIABLE_H

#include <string>

struct ParseVariable
{
	ParseVariable(const std::string& typeName, const std::string& fullName, bool isConst, bool isStatic);

	const std::string typeName;
	const std::string fullName;
	const bool isConst;
	const bool isStatic;
};

#endif // PARSE_VARIABLE_H
