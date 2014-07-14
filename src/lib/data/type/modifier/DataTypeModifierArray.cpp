#include "data/type/modifier/DataTypeModifierArray.h"

DataTypeModifierArray::DataTypeModifierArray()
{
}

DataTypeModifierArray::~DataTypeModifierArray()
{
}

std::shared_ptr<DataTypeModifier> DataTypeModifierArray::doCopy() const
{
	return std::make_shared<DataTypeModifierArray>();
}

void DataTypeModifierArray::doApplyTo(std::string& typeName) const
{
	typeName.append(" []");
}
