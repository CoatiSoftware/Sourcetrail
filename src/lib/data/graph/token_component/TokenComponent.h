#ifndef TOKEN_COMPONENT_H
#define TOKEN_COMPONENT_H

#include <memory>

class TokenComponent
{
public:
	virtual ~TokenComponent();
	virtual std::shared_ptr<TokenComponent> copy() const = 0;
};

#endif	  // TOKEN_COMPONENT_H
