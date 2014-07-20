#ifndef EDGE_COMPONENT_DATA_TYPE_H
#define EDGE_COMPONENT_DATA_TYPE_H

#include "data/graph/edgeComponent/EdgeComponent.h"
#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"

class DataType;

class EdgeComponentDataType: public EdgeComponent
{
public:
	EdgeComponentDataType(const DataTypeQualifierList qualifierList, const DataTypeModifierStack modifierStack);
	virtual ~EdgeComponentDataType();

	virtual std::shared_ptr<EdgeComponent> copy() const;

	DataType getDataType() const;

private:
	const DataTypeQualifierList m_qualifierList;
	const DataTypeModifierStack m_modifierStack;
};

#endif // EDGE_COMPONENT_DATA_TYPE_H
