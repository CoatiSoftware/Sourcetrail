#ifndef DATA_TYPE_QUALIFIER_H
#define DATA_TYPE_QUALIFIER_H

#include <memory>
#include <string>

#include "clang/AST/Type.h"

class DataTypeQualifierList
{
public:
	enum QualifierType
	{
		QUALIFIER_NONE = 0,
		QUALIFIER_CONST = 1
	};

	DataTypeQualifierList();
	~DataTypeQualifierList();

	void addQualifier(QualifierType qualifier);
	void removeQualifier(QualifierType qualifier);
	bool hasQualifier(QualifierType qualifier) const;

	std::string applyTo(const std::string& typeName) const;

private:
	char m_qualifiers;
};

#endif // DATA_TYPE_H
