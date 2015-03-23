#include "data/type/ReferenceModifiedDataType.h"

ReferenceModifiedDataType::ReferenceModifiedDataType(std::shared_ptr<DataType> dataType)
	: ModifiedDataType(dataType)
{
}

ReferenceModifiedDataType::~ReferenceModifiedDataType()
{
}

void ReferenceModifiedDataType::applyModifier(std::string& typeName) const
{
	typeName.append(" &");
}
