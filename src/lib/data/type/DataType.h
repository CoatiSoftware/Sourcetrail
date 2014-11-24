#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <memory>
#include <string>

#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"

class DataType
{
public:
	DataType(const std::vector<std::string>& typeNameHierarchy);
	DataType(const std::vector<std::string>& typeNameHierarchy, const DataTypeQualifierList& qualifierList);
	DataType(
		const std::vector<std::string>& typeNameHierarchy, const DataTypeQualifierList& qualifierList,
		const DataTypeModifierStack& modifierStack
	);
	~DataType();

	DataTypeQualifierList getQualifierList() const;
	DataTypeModifierStack getModifierStack() const;

	std::string getFullTypeName() const;
	std::string getRawTypeName() const;
	std::vector<std::string> getTypeNameHierarchy() const;

private:
	const std::vector<std::string> m_typeNameHierarchy;
	const DataTypeQualifierList m_qualifierList;
	const DataTypeModifierStack m_modifierStack;
};

#endif // DATA_TYPE_H
