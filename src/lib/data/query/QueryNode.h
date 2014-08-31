#ifndef QUERY_NODE_H
#define QUERY_NODE_H

#include <ostream>

class QueryNode
{
public:
	QueryNode();
	virtual ~QueryNode();

	virtual bool isCommand() const = 0;
	virtual bool isOperator() const = 0;
	virtual bool isToken() const = 0;

	virtual bool isComplete() const = 0;

	virtual void print(std::ostream& ostream) const = 0;
	virtual void print(std::ostream& ostream, int n) const;

	bool isGroup() const;
	void setIsGroup(bool isGroup);

	void setIsComplete(bool isComplete);

private:
	bool m_isGroup;
	bool m_isComplete;
};

#endif // QUERY_NODE_H
