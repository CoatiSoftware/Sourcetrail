#ifndef QUERY_TOKEN_H
#define QUERY_TOKEN_H

#include <set>
#include <string>

#include "data/query/QueryNode.h"
#include "utility/types.h"

class QueryToken
	: public QueryNode
{
public:
	QueryToken(const std::string& name);
	~QueryToken();

	virtual bool isCommand() const;
	virtual bool isOperator() const;
	virtual bool isToken() const;

	virtual bool derivedIsComplete() const;

	virtual void print(std::ostream& ostream) const;

	const std::string& getTokenName() const;
	const std::set<Id>& getTokenIds() const;

	static const char DELIMITER;
	static const char BOUNDARY;

private:
	std::string m_tokenName;
	std::set<Id> m_tokenIds;
};

#endif // QUERY_TOKEN_H
