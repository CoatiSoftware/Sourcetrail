#include "data/type/modifier/DataTypeModifierPointer.h"

DataTypeModifierPointer::DataTypeModifierPointer()
{
}

DataTypeModifierPointer::~DataTypeModifierPointer()
{
}

std::shared_ptr<DataTypeModifier> DataTypeModifierPointer::copy() const
{
	return std::make_shared<DataTypeModifierPointer>();
}

void DataTypeModifierPointer::doApplyTo(std::string& typeName) const
{
	typeName.append(" *");
}
