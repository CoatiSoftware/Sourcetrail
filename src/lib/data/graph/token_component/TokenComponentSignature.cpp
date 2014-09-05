#include "data/graph/token_component/TokenComponentSignature.h"

#include "utility/text/Dictionary.h"

std::shared_ptr<TokenComponentSignature> TokenComponentSignature::create(const std::string& signature)
{
	return std::shared_ptr<TokenComponentSignature>(
		new TokenComponentSignature(Dictionary::getInstance()->getWordId(signature)));
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
	return Dictionary::getInstance()->getWord(m_wordId);
}

bool TokenComponentSignature::operator==(const TokenComponentSignature& other) const
{
	return m_wordId == other.m_wordId;
}

TokenComponentSignature::TokenComponentSignature(Id wordId)
	: m_wordId(wordId)
{
}
