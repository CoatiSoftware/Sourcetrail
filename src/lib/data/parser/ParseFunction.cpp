#include "data/parser/ParseFunction.h"

#include "utility/utilityString.h"

ParseFunction::ParseFunction(
	const ParseTypeUsage& returnType,
	const NameHierarchy& nameHierarchy,
	const std::vector<ParseTypeUsage>& parameters,
	bool isStatic,
	bool isConst
)
	: returnType(returnType)
	, nameHierarchy(nameHierarchy)
	, parameters(parameters)
	, isStatic(isStatic)
	, isConst(isConst)
{
}

std::string ParseFunction::getFullName() const
{
	return nameHierarchy.getQualifiedNameWithSignature();
}
