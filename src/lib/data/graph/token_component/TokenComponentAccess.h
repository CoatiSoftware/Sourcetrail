#ifndef TOKEN_COMPONENT_ACCESS_H
#define TOKEN_COMPONENT_ACCESS_H

#include <string>

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentAccess
	: public TokenComponent
{
public:
	enum AccessType
	{
		ACCESS_PUBLIC,
		ACCESS_PROTECTED,
		ACCESS_PRIVATE,
		ACCESS_NONE
	};

	TokenComponentAccess(AccessType access);
	virtual ~TokenComponentAccess();

	virtual std::shared_ptr<TokenComponent> copy() const;

	AccessType getAccess() const;
	std::string getAccessString() const;

private:
	const AccessType m_access;
};

#endif // TOKEN_COMPONENT_ACCESS_H
