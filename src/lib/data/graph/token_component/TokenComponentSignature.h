#ifndef TOKEN_COMPONENT_SIGNATURE_H
#define TOKEN_COMPONENT_SIGNATURE_H

#include <memory>
#include <string>

#include "data/graph/token_component/TokenComponent.h"
#include "utility/types.h"

class TokenComponentSignature
	: public TokenComponent
{
public:
	static std::shared_ptr<TokenComponentSignature> create(const std::string& signature);

	virtual ~TokenComponentSignature();

	virtual std::shared_ptr<TokenComponent> copy() const;

	const std::string& getSignature() const;

	bool operator==(const TokenComponentSignature& other) const;

private:
	TokenComponentSignature(Id wordId);

	const Id m_wordId;
};

#endif // TOKEN_COMPONENT_SIGNATURE_H
