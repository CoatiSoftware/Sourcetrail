#ifndef PARSE_VARIABLE_H
#define PARSE_VARIABLE_H

#include <string>

#include "data/name/NameHierarchy.h"
#include "data/parser/ParseTypeUsage.h"

struct ParseVariable
{
	ParseVariable(const ParseTypeUsage& type, const NameHierarchy& nameHierarchy, bool isStatic);

	std::string getFullName() const;

	const ParseTypeUsage type;
	const NameHierarchy nameHierarchy;
	const bool isStatic;
};

#endif // PARSE_VARIABLE_H
