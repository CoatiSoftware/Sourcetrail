#include "data/graph/token_component/TokenComponentSignature.h"

TokenComponentSignature::TokenComponentSignature(Id wordId)
	: m_wordId(wordId)
{
}

TokenComponentSignature::~TokenComponentSignature()
{
}

std::shared_ptr<TokenComponent> TokenComponentSignature::copy() const
{
	return std::make_shared<TokenComponentSignature>(*this);
}

Id TokenComponentSignature::getWordId() const
{
	return m_wordId;
}

bool TokenComponentSignature::operator==(const TokenComponentSignature& other) const
{
	return m_wordId == other.m_wordId;
}
