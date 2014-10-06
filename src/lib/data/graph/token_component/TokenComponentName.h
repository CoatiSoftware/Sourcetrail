#ifndef TOKEN_COMPONENT_NAME_H
#define TOKEN_COMPONENT_NAME_H

#include <string>

#include "data/graph/token_component/TokenComponent.h"
#include "data/SearchIndex.h"

class TokenComponentName
	: public TokenComponent
{
public:
	TokenComponentName();
	virtual ~TokenComponentName();

	std::shared_ptr<TokenComponentName> copyComponentName() const;

	virtual std::string getName() const = 0;
	virtual std::string getFullName() const = 0;
};


class TokenComponentNameReferenced
	: public TokenComponentName
{
public:
	TokenComponentNameReferenced(const SearchIndex::SearchNode* searchNode);
	virtual ~TokenComponentNameReferenced();

	virtual std::shared_ptr<TokenComponent> copy() const;

	virtual std::string getName() const;
	virtual std::string getFullName() const;

private:
	const SearchIndex::SearchNode* m_searchNode;
};


class TokenComponentNameCached
	: public TokenComponentName
{
public:
	TokenComponentNameCached(const std::string& fullName);
	virtual ~TokenComponentNameCached();

	virtual std::shared_ptr<TokenComponent> copy() const;

	virtual std::string getName() const;
	virtual std::string getFullName() const;

private:
	const std::string m_fullName;
};

#endif // TOKEN_COMPONENT_NAME_H
