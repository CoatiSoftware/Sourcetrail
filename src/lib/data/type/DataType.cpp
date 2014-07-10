#include "data/type/DataType.h"

DataType::DataType(
		const std::string& typeName, const DataTypeQualifierList qualifierList, const DataTypeModifierStack modifierStack
)
	: m_typeName(typeName)
	, m_qualifierList(qualifierList)
	, m_modifierStack(modifierStack)
{
}

DataType::~DataType()
{
}

DataTypeQualifierList DataType::getQualifierList() const
{
	return m_qualifierList;
}

DataTypeModifierStack DataType::getModifierStack() const
{
	return m_modifierStack;
}

std::string DataType::getFullTypeName() const
{
	return m_modifierStack.applyTo(m_qualifierList.applyTo(m_typeName));
}

std::string DataType::getRawTypeName() const
{
	return m_typeName;
}
