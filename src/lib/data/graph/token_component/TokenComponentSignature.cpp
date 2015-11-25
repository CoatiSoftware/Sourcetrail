#include "data/graph/token_component/TokenComponentSignature.h"

TokenComponentSignature::TokenComponentSignature(const std::string& signature)
	: m_signature(signature)
{
}

TokenComponentSignature::~TokenComponentSignature()
{
}

std::shared_ptr<TokenComponent> TokenComponentSignature::copy() const
{
	return std::make_shared<TokenComponentSignature>(*this);
}

const std::string& TokenComponentSignature::getSignature() const
{
	return m_signature;
}
