#include "data/parser/ParseVariable.h"

#include "utility/utilityString.h"

ParseVariable::ParseVariable(const ParseTypeUsage& type, const NameHierarchy& nameHierarchy, bool isStatic)
	: type(type)
	, nameHierarchy(nameHierarchy)
	, isStatic(isStatic)
{
}

std::string ParseVariable::getFullName() const
{
	return nameHierarchy.getQualifiedNameWithSignature();
}
