#ifndef NAMED_DATA_TYPE_H
#define NAMED_DATA_TYPE_H

#include "DataType.h"

class NamedDataType: public DataType
{
public:
	NamedDataType(const std::vector<std::string>& nameHierarchy);
	virtual ~NamedDataType();

	virtual std::string getFullTypeName() const;
	virtual std::string getRawTypeName() const;
	virtual const std::vector<std::string>& getTypeNameHierarchy() const;

private:
	const std::vector<std::string> m_nameHierarchy;
};

#endif // NAMED_DATA_TYPE_H

