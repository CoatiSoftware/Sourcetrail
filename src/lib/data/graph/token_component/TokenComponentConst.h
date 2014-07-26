#ifndef TOKEN_COMPONENT_CONST
#define TOKEN_COMPONENT_CONST

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentConst
	: public TokenComponent
{
public:
	virtual std::shared_ptr<TokenComponent> copy() const;
};

#endif // TOKEN_COMPONENT_CONST
