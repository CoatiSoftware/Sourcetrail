#ifndef TOKEN_COMPONENT_SIGNATURE_H
#define TOKEN_COMPONENT_SIGNATURE_H

#include "data/graph/token_component/TokenComponent.h"
#include "utility/types.h"

class TokenComponentSignature
	: public TokenComponent
{
public:
	TokenComponentSignature(Id wordId);
	virtual ~TokenComponentSignature();

	virtual std::shared_ptr<TokenComponent> copy() const;

	Id getWordId() const;

	bool operator==(const TokenComponentSignature& other) const;

private:
	const Id m_wordId;
};

#endif // TOKEN_COMPONENT_SIGNATURE_H
