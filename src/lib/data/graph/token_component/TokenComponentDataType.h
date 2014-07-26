#ifndef TOKEN_COMPONENT_DATA_TYPE
#define TOKEN_COMPONENT_DATA_TYPE

#include "data/graph/token_component/TokenComponent.h"
#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"

class DataType;

class TokenComponentDataType
	: public TokenComponent
{
public:
	TokenComponentDataType(const DataTypeQualifierList qualifierList, const DataTypeModifierStack modifierStack);
	virtual ~TokenComponentDataType();

	virtual std::shared_ptr<TokenComponent> copy() const;

	DataType getDataType(const std::string& typeName) const;
	std::string getQualifiedTypeName(const std::string& typeName) const;

private:
	const DataTypeQualifierList m_qualifierList;
	const DataTypeModifierStack m_modifierStack;
};

#endif // TOKEN_COMPONENT_DATA_TYPE
