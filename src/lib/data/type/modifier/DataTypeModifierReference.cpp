#include "data/type/modifier/DataTypeModifierReference.h"

DataTypeModifierReference::DataTypeModifierReference()
{
}

DataTypeModifierReference::~DataTypeModifierReference()
{
}

std::shared_ptr<DataTypeModifier> DataTypeModifierReference::doCopy() const
{
	return std::make_shared<DataTypeModifierReference>();
}

void DataTypeModifierReference::doApplyTo(std::string& typeName) const
{
	typeName.append(" &");
}
