#include "data/type/DataType.h"

#include "utility/utilityString.h"

DataType::DataType(const std::vector<std::string>& typeNameHierarchy)
	: m_typeNameHierarchy(typeNameHierarchy)
{
}

DataType::DataType(const std::vector<std::string>& typeNameHierarchy, const DataTypeQualifierList& qualifierList)
	: m_typeNameHierarchy(typeNameHierarchy)
	, m_qualifierList(qualifierList)
{
}

DataType::DataType(
	const std::vector<std::string>& typeNameHierarchy, const DataTypeQualifierList& qualifierList,
	const DataTypeModifierStack& modifierStack
)
	: m_typeNameHierarchy(typeNameHierarchy)
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
	return m_modifierStack.applyTo(m_qualifierList.applyTo(getRawTypeName()));
}

std::string DataType::getRawTypeName() const
{
	return utility::join(m_typeNameHierarchy, "::");
}

std::vector<std::string> DataType::getTypeNameHierarchy() const
{
	return m_typeNameHierarchy;
}
