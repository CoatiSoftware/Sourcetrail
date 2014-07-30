#include "data/parser/ParseFunction.h"

ParseFunction::ParseFunction(
	const ParseTypeUsage& returnType,
	const std::string& fullName,
	const std::vector<ParseTypeUsage>& parameters,
	bool isStatic,
	bool isConst
)
	: returnType(returnType)
	, fullName(fullName)
	, parameters(parameters)
	, isStatic(isStatic)
	, isConst(isConst)
{
}
