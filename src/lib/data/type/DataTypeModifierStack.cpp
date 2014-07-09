#include "data/type/DataTypeModifierStack.h"

DataTypeModifierStack::DataTypeModifierStack()
{
}

//DataTypeModifierStack::DataTypeModifierStack(const DataTypeModifierStack& o)
//{
//	for (std::shared_ptr<DataTypeModifier> modifier: o.m_modifiers)
//	{
//		m_modifiers.push_back(modifier.);
//	}
//}


DataTypeModifierStack::~DataTypeModifierStack()
{
}


void DataTypeModifierStack::push(std::shared_ptr<DataTypeModifier> modifier)
{
	m_modifiers.push_back(modifier);
}


std::string DataTypeModifierStack::applyTo(const std::string& typeName) const
{
	std::string modifiedTypeName = typeName;
	for (std::shared_ptr<DataTypeModifier> modifier: m_modifiers)
	{
		modifier->applyTo(modifiedTypeName);
	}
	return modifiedTypeName;
}
