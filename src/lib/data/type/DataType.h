#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <memory>
#include <string>

#include "clang/AST/Type.h"

#include "data/type/DataTypeModifierStack.h"

class DataType
{
public:
	DataType(clang::QualType qualType);
	DataType(const std::string& typeName);
	~DataType();

	std::string getFullTypeName() const;
	std::string getRawTypeName() const;

private:
	std::string m_typeName;
	DataTypeQualifierList m_qualifierList;
	DataTypeModifierStack m_modifierStack;
};

#endif // DATA_TYPE_H
