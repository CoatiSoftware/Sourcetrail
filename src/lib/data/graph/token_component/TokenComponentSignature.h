#ifndef TOKEN_COMPONENT_SIGNATURE_H
#define TOKEN_COMPONENT_SIGNATURE_H

#include <string>

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentSignature
	: public TokenComponent
{
public:
	TokenComponentSignature(std::string signature);
	virtual ~TokenComponentSignature();

	virtual std::shared_ptr<TokenComponent> copy() const;

	const std::string& getSignature() const;

private:
	const std::string m_signature;
};

#endif // TOKEN_COMPONENT_SIGNATURE_H
