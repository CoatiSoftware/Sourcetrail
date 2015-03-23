#include "data/type/ArrayModifiedDataType.h"

ArrayModifiedDataType::ArrayModifiedDataType(std::shared_ptr<DataType> dataType)
	: ModifiedDataType(dataType)
{
}

ArrayModifiedDataType::~ArrayModifiedDataType()
{
}

void ArrayModifiedDataType::applyModifier(std::string& typeName) const
{
	typeName.append(" []");
}
