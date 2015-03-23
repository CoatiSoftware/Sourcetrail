#ifndef POINTER_MODIFIED_DATA_TYPE_H
#define POINTER_MODIFIED_DATA_TYPE_H

#include "ModifiedDataType.h"

class PointerModifiedDataType: public ModifiedDataType
{
public:
	PointerModifiedDataType(std::shared_ptr<DataType> dataType);
	virtual ~PointerModifiedDataType();

protected:
	virtual void applyModifier(std::string& typeName) const;
};

#endif // POINTER_MODIFIED_DATA_TYPE_H