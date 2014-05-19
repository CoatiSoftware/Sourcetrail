#include "data/parser/ParseVariable.h"

ParseVariable::ParseVariable(const std::string& typeName, const std::string& fullName, bool isConst, bool isStatic)
	: typeName(typeName)
	, fullName(fullName)
	, isConst(isConst)
	, isStatic(isStatic)
{
}
