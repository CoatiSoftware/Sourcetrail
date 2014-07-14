#ifndef DATA_TYPE_MODIFIER_REFERENCE_H
#define DATA_TYPE_MODIFIER_REFERENCE_H

#include "data/type/modifier/DataTypeModifier.h"

class DataTypeModifierReference: public DataTypeModifier
{
public:
	DataTypeModifierReference();
	virtual ~DataTypeModifierReference();

private:
	virtual std::shared_ptr<DataTypeModifier> doCopy() const;
	virtual void doApplyTo(std::string& typeName) const;
};

#endif // DATA_TYPE_MODIFIER_REFERENCE_H
