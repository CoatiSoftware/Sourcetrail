#include "data/graph/token_component/TokenComponentName.h"

#include "utility/utilityString.h"

TokenComponentName::TokenComponentName()
{
}

TokenComponentName::~TokenComponentName()
{
}

std::shared_ptr<TokenComponentName> TokenComponentName::copyComponentName() const
{
	return std::dynamic_pointer_cast<TokenComponentName>(copy());
}


TokenComponentNameReferenced::TokenComponentNameReferenced(const SearchIndex::SearchNode* searchNode)
	: m_searchNode(searchNode)
{
}

TokenComponentNameReferenced::~TokenComponentNameReferenced()
{
}

std::shared_ptr<TokenComponent> TokenComponentNameReferenced::copy() const
{
	return std::make_shared<TokenComponentNameCached>(getFullName());
}

const std::string& TokenComponentNameReferenced::getName() const
{
	return m_searchNode->getName();
}

std::string TokenComponentNameReferenced::getFullName() const
{
	return m_searchNode->getFullName();
}


TokenComponentNameCached::TokenComponentNameCached(const std::string& fullName)
	: m_fullName(fullName)
{
}

TokenComponentNameCached::~TokenComponentNameCached()
{
}

std::shared_ptr<TokenComponent> TokenComponentNameCached::copy() const
{
	return std::make_shared<TokenComponentNameCached>(m_fullName);
}

const std::string& TokenComponentNameCached::getName() const
{
	return utility::split<std::deque<std::string>>(m_fullName, SearchIndex::DELIMITER).back();
}

std::string TokenComponentNameCached::getFullName() const
{
	return m_fullName;
}
