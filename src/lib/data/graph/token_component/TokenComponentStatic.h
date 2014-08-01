#ifndef TOKEN_COMPONENT_STATIC_H
#define TOKEN_COMPONENT_STATIC_H

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentStatic
	: public TokenComponent
{
public:
	virtual std::shared_ptr<TokenComponent> copy() const;
};

#endif // TOKEN_COMPONENT_STATIC_H
