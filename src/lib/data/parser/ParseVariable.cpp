#include "data/parser/ParseVariable.h"

ParseVariable::ParseVariable(const ParseTypeUsage& type, const std::string& fullName, bool isStatic)
	: type(type)
	, fullName(fullName)
	, isStatic(isStatic)
{
}
