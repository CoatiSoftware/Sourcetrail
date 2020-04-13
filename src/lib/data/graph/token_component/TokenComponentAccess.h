#ifndef TOKEN_COMPONENT_ACCESS_H
#define TOKEN_COMPONENT_ACCESS_H

#include <string>

#include "../../parser/AccessKind.h"
#include "TokenComponent.h"

class TokenComponentAccess: public TokenComponent
{
public:
	static std::wstring getAccessString(AccessKind access);

	TokenComponentAccess(AccessKind access);
	virtual ~TokenComponentAccess();

	virtual std::shared_ptr<TokenComponent> copy() const;

	AccessKind getAccess() const;
	std::wstring getAccessString() const;

private:
	const AccessKind m_access;
};

#endif	  // TOKEN_COMPONENT_ACCESS_H
