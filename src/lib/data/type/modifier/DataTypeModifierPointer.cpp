#include "data/type/modifier/DataTypeModifierPointer.h"

DataTypeModifierPointer::DataTypeModifierPointer()
{
}

DataTypeModifierPointer::~DataTypeModifierPointer()
{
}

void DataTypeModifierPointer::doApplyTo(std::string& typeName) const
{
	typeName.append(" *");
}
