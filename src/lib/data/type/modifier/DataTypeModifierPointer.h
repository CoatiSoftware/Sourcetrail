#ifndef DATA_TYPE_MODIFIER_POINTER_H
#define DATA_TYPE_MODIFIER_POINTER_H

#include "data/type/modifier/DataTypeModifier.h"

class DataTypeModifierPointer: public DataTypeModifier
{
public:
	DataTypeModifierPointer();
	virtual ~DataTypeModifierPointer();

	virtual void applyTo(std::string& typeName) const;


};

#endif // DATA_TYPE_MODIFIER_POINTER_H
