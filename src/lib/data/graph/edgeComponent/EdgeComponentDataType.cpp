#include "data/graph/edgeComponent/EdgeComponentDataType.h"

#include "data/type/DataType.h"
#include "data/graph/Node.h"

EdgeComponentDataType::EdgeComponentDataType(const DataTypeQualifierList qualifierList, const DataTypeModifierStack modifierStack)
	: m_qualifierList(qualifierList)
	, m_modifierStack(modifierStack)
{
}

EdgeComponentDataType::~EdgeComponentDataType()
{
}

std::shared_ptr<EdgeComponent> EdgeComponentDataType::copy() const
{
	return std::make_shared<EdgeComponentDataType>(m_qualifierList, m_modifierStack);
}

DataType EdgeComponentDataType::getDataType() const
{
	return DataType(getEdge()->getTo()->getName(), m_qualifierList, m_modifierStack);
}
