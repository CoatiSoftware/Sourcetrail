#ifndef PARSE_FUNCTION_H
#define PARSE_FUNCTION_H

#include <string>

#include "data/parser/ParseTypeUsage.h"

struct ParseFunction
{
	ParseFunction(
		const ParseTypeUsage& returnType,
		const std::string& fullName,
		const std::vector<ParseTypeUsage>& parameters,
		bool isStatic = false,
		bool isConst = false
	);

	const ParseTypeUsage returnType;
	const std::string fullName;
	const std::vector<ParseTypeUsage> parameters;
	const bool isStatic;
	const bool isConst;
};

#endif // PARSE_FUNCTION_H
