#include "data/type/modifier/DataTypeModifierPointer.h"

DataTypeModifierPointer::DataTypeModifierPointer()
{
}

DataTypeModifierPointer::~DataTypeModifierPointer()
{
}

void DataTypeModifierPointer::applyTo(std::string& typeName) const
{
	typeName.append(" *");
}
