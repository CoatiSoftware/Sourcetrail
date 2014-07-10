#ifndef PARSE_VARIABLE_H
#define PARSE_VARIABLE_H

#include <string>

#include "data/type/DataType.h"

struct ParseVariable
{
	ParseVariable(const DataType& type, const std::string& fullName, bool isStatic);

	const DataType type;
	const std::string fullName;
	const bool isStatic;
};

#endif // PARSE_VARIABLE_H
