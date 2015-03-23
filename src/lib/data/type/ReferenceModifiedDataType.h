#ifndef REFERENCE_MODIFIED_DATA_TYPE_H
#define REFERENCE_MODIFIED_DATA_TYPE_H

#include "ModifiedDataType.h"

class ReferenceModifiedDataType: public ModifiedDataType
{
public:
	ReferenceModifiedDataType(std::shared_ptr<DataType> dataType);
	virtual ~ReferenceModifiedDataType();

protected:
	virtual void applyModifier(std::string& typeName) const;
};

#endif // REFERENCE_MODIFIED_DATA_TYPE_H