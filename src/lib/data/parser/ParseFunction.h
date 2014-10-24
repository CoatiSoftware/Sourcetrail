#ifndef PARSE_FUNCTION_H
#define PARSE_FUNCTION_H

#include <string>

#include "data/parser/ParseTypeUsage.h"

struct ParseFunction
{
	ParseFunction(
		const ParseTypeUsage& returnType,
		const std::vector<std::string>& nameHierarchy,
		const std::vector<ParseTypeUsage>& parameters,
		bool isStatic = false,
		bool isConst = false
	);

	std::string getFullName() const;

	const ParseTypeUsage returnType;
	const std::vector<std::string> nameHierarchy;
	const std::vector<ParseTypeUsage> parameters;
	const bool isStatic;
	const bool isConst;
};

#endif // PARSE_FUNCTION_H
