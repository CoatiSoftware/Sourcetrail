#ifndef MODIFIED_DATA_TYPE_H
#define MODIFIED_DATA_TYPE_H

#include <memory>

#include "DataType.h"

class ModifiedDataType: public DataType
{
public:
	ModifiedDataType(std::shared_ptr<DataType> dataType);
	virtual ~ModifiedDataType();

	virtual std::string getFullTypeName() const;
	virtual std::string getRawTypeName() const;
	virtual const std::vector<std::string>& getTypeNameHierarchy() const;

protected:
	virtual void applyModifier(std::string& typeName) const = 0;

private:
	const std::shared_ptr<DataType> m_dataType;
};

#endif // MODIFIED_DATA_TYPE_H