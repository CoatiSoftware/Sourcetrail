#ifndef TOKEN_COMPONENT_STATIC
#define TOKEN_COMPONENT_STATIC

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentStatic
	: public TokenComponent
{
public:
	virtual std::shared_ptr<TokenComponent> copy() const;
};

#endif // TOKEN_COMPONENT_STATIC
