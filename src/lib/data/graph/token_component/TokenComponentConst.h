#ifndef TOKEN_COMPONENT_CONST_H
#define TOKEN_COMPONENT_CONST_H

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentConst
	: public TokenComponent
{
public:
	virtual std::shared_ptr<TokenComponent> copy() const;
};

#endif // TOKEN_COMPONENT_CONST_H
