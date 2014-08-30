#include "data/graph/token_component/TokenComponentDataType.h"

#include "data/type/DataType.h"

TokenComponentDataType::TokenComponentDataType(
	const DataTypeQualifierList qualifierList, const DataTypeModifierStack modifierStack
)
	: m_qualifierList(qualifierList)
	, m_modifierStack(modifierStack)
{
}

TokenComponentDataType::~TokenComponentDataType()
{
}

std::shared_ptr<TokenComponent> TokenComponentDataType::copy() const
{
	return std::make_shared<TokenComponentDataType>(*this);
}

DataType TokenComponentDataType::getDataType(const std::string& typeName) const
{
	return DataType(typeName, m_qualifierList, m_modifierStack);
}

std::string TokenComponentDataType::getQualifiedTypeName(const std::string& typeName) const
{
	return m_modifierStack.applyTo(m_qualifierList.applyTo(typeName));
}

bool TokenComponentDataType::isConstQualified() const
{
	return m_qualifierList.hasQualifier(DataTypeQualifierList::QUALIFIER_CONST);
}
