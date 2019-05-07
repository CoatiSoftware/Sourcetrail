#ifndef TOKEN_COMPONENT_IS_AMBIGUOUS_H
#define TOKEN_COMPONENT_IS_AMBIGUOUS_H

#include "TokenComponent.h"

class TokenComponentIsAmbiguous
	: public TokenComponent
{
public:
	inline virtual std::shared_ptr<TokenComponent> copy() const
	{
		return std::make_shared<TokenComponentIsAmbiguous>(*this);
	}
};

#endif // TOKEN_COMPONENT_IS_AMBIGUOUS_H
