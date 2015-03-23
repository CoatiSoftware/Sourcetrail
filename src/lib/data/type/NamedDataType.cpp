#include "data/type/NamedDataType.h"

#include "utility/utilityString.h"

NamedDataType::NamedDataType(const std::vector<std::string>& nameHierarchy)
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
	return utility::join(m_nameHierarchy, "::");
}

const std::vector<std::string>& NamedDataType::getTypeNameHierarchy() const
{
	return m_nameHierarchy;
}
