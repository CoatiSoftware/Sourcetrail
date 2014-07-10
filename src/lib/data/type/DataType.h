#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <memory>
#include <string>

#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"

class DataType
{
public:
	DataType(
		const std::string& typeName, const DataTypeQualifierList qualifierList, const DataTypeModifierStack modifierStack
	);
	~DataType();

	DataTypeQualifierList getQualifierList() const;
	DataTypeModifierStack getModifierStack() const;

	std::string getFullTypeName() const;
	std::string getRawTypeName() const;

private:
	const std::string m_typeName;
	const DataTypeQualifierList m_qualifierList;
	const DataTypeModifierStack m_modifierStack;
};

#endif // DATA_TYPE_H
