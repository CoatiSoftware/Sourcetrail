#ifndef ARRAY_MODIFIED_DATA_TYPE_H
#define ARRAY_MODIFIED_DATA_TYPE_H

#include "ModifiedDataType.h"

class ArrayModifiedDataType: public ModifiedDataType
{
public:
	ArrayModifiedDataType(std::shared_ptr<DataType> dataType);
	virtual ~ArrayModifiedDataType();

protected:
	virtual void applyModifier(std::string& typeName) const;
};

#endif // ARRAY_MODIFIED_DATA_TYPE_H