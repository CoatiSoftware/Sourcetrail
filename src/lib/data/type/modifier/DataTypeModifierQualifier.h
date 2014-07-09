#ifndef DATA_TYPE_MODIFIER_QUALIFIER_H
#define DATA_TYPE_MODIFIER_QUALIFIER_H

#include "data/type/modifier/DataTypeModifier.h"

class DataTypeModifierQualifier: public DataTypeModifier
{
public:
	enum QualifierType
	{
		QUALIFIER_NONE = 0,
		QUALIFIER_CONST = 1
	};

	DataTypeModifierQualifier();
	virtual ~DataTypeModifierQualifier();

	void addQualifier(QualifierType qualifier);
	void removeQualifier(QualifierType qualifier);
	bool hasQualifier(QualifierType qualifier) const;

	virtual void applyTo(std::string& typeName) const;

private:
	char m_qualifiers;
};

#endif // DATA_TYPE_MODIFIER_QUALIFIER_H
