#ifndef DATA_TYPE_MODIFIER_H
#define DATA_TYPE_MODIFIER_H

#include <string>

#include "data/type/DataTypeQualifierList.h"

class DataTypeModifier
{
public:
	DataTypeModifier();
	virtual ~DataTypeModifier();

	std::shared_ptr<DataTypeModifier> copy() const;

	void applyTo(std::string& typeName) const;

	void addQualifier(DataTypeQualifierList::QualifierType qualifier);
	void removeQualifier(DataTypeQualifierList::QualifierType qualifier);
	bool hasQualifier(DataTypeQualifierList::QualifierType qualifier) const;

private:
	virtual std::shared_ptr<DataTypeModifier> doCopy() const = 0;
	virtual void doApplyTo(std::string& typeName) const = 0;

	DataTypeQualifierList m_qualifierList;
};

#endif // DATA_TYPE_MODIFIER_H
