#ifndef QUERY_NODE_H
#define QUERY_NODE_H

#include <ostream>
#include <string>

class QueryNode
{
public:
	enum QueryNodeType
	{
		QUERYNODETYPE_NONE,
		QUERYNODETYPE_TOKEN,
		QUERYNODETYPE_COMMAND,
		QUERYNODETYPE_OPERATOR
	};

	static std::string queryNodeTypeToString(const QueryNodeType& type);

	QueryNode(QueryNodeType queryNodeType = QUERYNODETYPE_NONE);
	virtual ~QueryNode();

	virtual bool isCommand() const = 0;
	virtual bool isOperator() const = 0;
	virtual bool isToken() const = 0;

	virtual bool derivedIsComplete() const = 0;


	virtual std::string getName() const = 0;

	virtual void print(std::ostream& ostream) const = 0;
	virtual void print(std::ostream& ostream, int n) const;

	bool isGroup() const;
	void setIsGroup(bool isGroup);

	bool isComplete() const;
	void setIsComplete(bool isComplete);

	QueryNodeType getQueryNodeType() const;
	std::string getQueryNodeTypeAsString() const;

private:
	QueryNodeType m_nodeType;
	bool m_isGroup;
	bool m_isComplete;
};

#endif // QUERY_NODE_H
