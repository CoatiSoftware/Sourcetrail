#include "data/type/NamedDataType.h"

#include "utility/utilityString.h"

NamedDataType::NamedDataType(const NameHierarchy& nameHierarchy)
	: m_nameHierarchy(nameHierarchy)
{
}

NamedDataType::~NamedDataType()
{
}

std::string NamedDataType::getFullTypeName() const
{
	std::string fullTypeName = getRawTypeName();
	applyQualifieres(fullTypeName);
	return fullTypeName;
}

std::string NamedDataType::getRawTypeName() const
{
	return m_nameHierarchy.getFullName();
}

const NameHierarchy& NamedDataType::getTypeNameHierarchy() const
{
	return m_nameHierarchy;
}
