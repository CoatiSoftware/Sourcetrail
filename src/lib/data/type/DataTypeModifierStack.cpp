#include "data/type/DataTypeModifierStack.h"

DataTypeModifierStack::DataTypeModifierStack()
{
}

DataTypeModifierStack::DataTypeModifierStack(const DataTypeModifierStack& o)
{
	for (std::shared_ptr<DataTypeModifier> modifier: o.m_modifiers)
	{
		m_modifiers.push_back(modifier->copy());
	}
}

DataTypeModifierStack& DataTypeModifierStack::operator=(const DataTypeModifierStack &o)
{
	if (this != &o)
	{
		m_modifiers.clear();
		for (std::shared_ptr<DataTypeModifier> modifier: o.m_modifiers)
		{
			m_modifiers.push_back(modifier->copy());
		}
	}
	return *this;
}

DataTypeModifierStack::~DataTypeModifierStack()
{
}

void DataTypeModifierStack::push(std::shared_ptr<DataTypeModifier> modifier)
{
	m_modifiers.push_back(modifier);
}

std::string DataTypeModifierStack::applyTo(const std::string& typeName) const
{
	typedef std::vector<std::shared_ptr<DataTypeModifier>>::const_reverse_iterator StackIterator;
	std::string modifiedTypeName = typeName;
	for (StackIterator it = m_modifiers.rbegin(); it != m_modifiers.rend(); it++)
	{
		(*it)->applyTo(modifiedTypeName);
	}
	return modifiedTypeName;
}
