#ifndef PARSE_VARIABLE_H
#define PARSE_VARIABLE_H

#include <string>

#include "data/parser/ParseTypeUsage.h"

struct ParseVariable
{
	ParseVariable(const ParseTypeUsage& type, const std::string& fullName, bool isStatic);

	const ParseTypeUsage type;
	const std::string fullName;
	const bool isStatic;
};

#endif // PARSE_VARIABLE_H
