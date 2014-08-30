#ifndef QUERY_TOKEN_H
#define QUERY_TOKEN_H

#include <string>

#include "data/query/QueryNode.h"

class QueryToken
	: public QueryNode
{
public:
	QueryToken(const std::string& name);
	~QueryToken();

	virtual bool isCommand() const;
	virtual bool isOperator() const;
	virtual bool isToken() const;
	virtual bool isComplete() const;

	virtual void print(std::ostream& ostream) const;

	const std::string& getName() const;

private:
	const std::string m_name;
};

#endif // QUERY_TOKEN_H
