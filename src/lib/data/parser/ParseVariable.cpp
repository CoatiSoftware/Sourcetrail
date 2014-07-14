#include "data/parser/ParseVariable.h"

ParseVariable::ParseVariable(const DataType& type, const std::string& fullName, bool isStatic)
	: type(type)
	, fullName(fullName)
	, isStatic(isStatic)
{
}
