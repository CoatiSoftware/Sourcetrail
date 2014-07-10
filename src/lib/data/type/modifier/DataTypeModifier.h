#ifndef DATA_TYPE_MODIFIER_H
#define DATA_TYPE_MODIFIER_H

#include <string>

#include "data/type/DataTypeQualifierList.h"

class DataTypeModifier
{
public:
	DataTypeModifier();
	virtual ~DataTypeModifier();

	virtual std::shared_ptr<DataTypeModifier> copy() const = 0;

	void applyTo(std::string& typeName) const;

	void addQualifier(DataTypeQualifierList::QualifierType qualifier);
	void removeQualifier(DataTypeQualifierList::QualifierType qualifier);
	bool hasQualifier(DataTypeQualifierList::QualifierType qualifier) const;

private:
	virtual void doApplyTo(std::string& typeName) const = 0;

	DataTypeQualifierList m_qualifierList;
};

#endif // DATA_TYPE_MODIFIER_H
