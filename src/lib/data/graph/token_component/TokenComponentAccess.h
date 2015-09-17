#ifndef TOKEN_COMPONENT_ACCESS_H
#define TOKEN_COMPONENT_ACCESS_H

#include <string>

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentAccess
	: public TokenComponent
{
public:
	enum AccessType : int
	{
		ACCESS_PUBLIC = 0x1,
		ACCESS_PROTECTED = 0x2,
		ACCESS_PRIVATE = 0x4,
		ACCESS_NONE = 0x0
	};

	static std::string getAccessString(AccessType access);

	static int typeToInt(AccessType type);
	static AccessType intToType(int value);

	TokenComponentAccess(AccessType access);
	virtual ~TokenComponentAccess();

	virtual std::shared_ptr<TokenComponent> copy() const;

	AccessType getAccess() const;
	std::string getAccessString() const;

private:
	const AccessType m_access;
};

#endif // TOKEN_COMPONENT_ACCESS_H
