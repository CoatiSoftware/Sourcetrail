#include "data/type/PointerModifiedDataType.h"

PointerModifiedDataType::PointerModifiedDataType(std::shared_ptr<DataType> dataType)
	: ModifiedDataType(dataType)
{
}

PointerModifiedDataType::~PointerModifiedDataType()
{
}

void PointerModifiedDataType::applyModifier(std::string& typeName) const
{
	typeName.append(" *");
}
