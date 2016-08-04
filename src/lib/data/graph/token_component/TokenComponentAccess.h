#ifndef TOKEN_COMPONENT_ACCESS_H
#define TOKEN_COMPONENT_ACCESS_H

#include <string>

#include "data/graph/token_component/TokenComponent.h"
#include "data/parser/AccessKind.h"

class TokenComponentAccess
	: public TokenComponent
{
public:
	static std::string getAccessString(AccessKind access);

	TokenComponentAccess(AccessKind access);
	virtual ~TokenComponentAccess();

	virtual std::shared_ptr<TokenComponent> copy() const;

	AccessKind getAccess() const;
	std::string getAccessString() const;

private:
	const AccessKind m_access;
};

#endif // TOKEN_COMPONENT_ACCESS_H
