#ifndef DATA_TYPE_MODIFIER_ARRAY_H
#define DATA_TYPE_MODIFIER_ARRAY_H

#include "data/type/modifier/DataTypeModifier.h"

class DataTypeModifierArray: public DataTypeModifier
{
public:
	DataTypeModifierArray();
	virtual ~DataTypeModifierArray();

private:
	virtual std::shared_ptr<DataTypeModifier> doCopy() const;
	virtual void doApplyTo(std::string& typeName) const;
};

#endif // DATA_TYPE_MODIFIER_ARRAY_H
