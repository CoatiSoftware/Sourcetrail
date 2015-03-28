#include "data/type/ModifiedDataType.h"

ModifiedDataType::ModifiedDataType(std::shared_ptr<DataType> dataType)
	: m_dataType(dataType)
{
}

ModifiedDataType::~ModifiedDataType()
{
}

std::string ModifiedDataType::getFullTypeName() const
{
	std::string fullTypeName = m_dataType->getFullTypeName();
	applyModifier(fullTypeName);
	applyQualifieres(fullTypeName);
	return fullTypeName;
}

std::string ModifiedDataType::getRawTypeName() const
{
	return m_dataType->getRawTypeName();
}

const NameHierarchy& ModifiedDataType::getTypeNameHierarchy() const
{
	return m_dataType->getTypeNameHierarchy();
}
