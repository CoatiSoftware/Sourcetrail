#ifndef DATA_TYPE_MODIFIER_STACK_H
#define DATA_TYPE_MODIFIER_STACK_H

#include <memory>
#include <string>
#include <vector>

#include "data/type/modifier/DataTypeModifier.h"

class DataTypeModifierStack
{
public:
	DataTypeModifierStack();
	DataTypeModifierStack(const DataTypeModifierStack& o);
	~DataTypeModifierStack();

	void push(std::shared_ptr<DataTypeModifier> modifier);

	std::string applyTo(const std::string& typeName) const;

private:
	std::vector<std::shared_ptr<DataTypeModifier>> m_modifiers;
};

#endif // DATA_TYPE_MODIFIER_STACK_H
