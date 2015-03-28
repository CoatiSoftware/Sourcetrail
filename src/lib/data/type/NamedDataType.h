#ifndef NAMED_DATA_TYPE_H
#define NAMED_DATA_TYPE_H

#include "data/type/DataType.h"

class NamedDataType: public DataType
{
public:
	NamedDataType(const NameHierarchy& nameHierarchy);
	virtual ~NamedDataType();

	virtual std::string getFullTypeName() const;
	virtual std::string getRawTypeName() const;
	virtual const NameHierarchy& getTypeNameHierarchy() const;

private:
	const NameHierarchy m_nameHierarchy;
};

#endif // NAMED_DATA_TYPE_H

