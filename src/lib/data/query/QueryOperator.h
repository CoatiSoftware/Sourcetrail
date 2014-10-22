#ifndef QUERY_OPERATOR_H
#define QUERY_OPERATOR_H

#include <map>
#include <memory>

#include "data/query/QueryNode.h"

class QueryOperator
	: public QueryNode
{
public:
	enum OperatorType
	{
		OPERATOR_NONE,

		OPERATOR_NOT,
		OPERATOR_SUB,
		OPERATOR_HAS,
		OPERATOR_AND,
		OPERATOR_OR,

		OPERATOR_TOKEN,
		OPERATOR_COMMAND,

		OPERATOR_GROUP_OPEN,
		OPERATOR_GROUP_CLOSE
	};

	static const std::map<char, OperatorType>& getOperatorTypeMap();
	static OperatorType getOperatorType(char c);
	static char getOperator(OperatorType t);

	QueryOperator(OperatorType type, bool isImplicit);
	~QueryOperator();

	virtual bool isCommand() const;
	virtual bool isOperator() const;
	virtual bool isToken() const;

	virtual bool derivedIsComplete() const;

	virtual std::string getName() const;

	virtual void print(std::ostream& ostream) const;
	virtual void print(std::ostream& ostream, int n) const;

	std::shared_ptr<QueryNode> getLeft() const;
	void setLeft(std::shared_ptr<QueryNode> node);

	std::shared_ptr<QueryNode> getRight() const;
	void setRight(std::shared_ptr<QueryNode> node);

	OperatorType getType() const;

	bool lowerPrecedence(const QueryOperator& other);

	bool isImplicit() const;

private:
	std::shared_ptr<QueryNode> m_left;
	std::shared_ptr<QueryNode> m_right;

	const OperatorType m_type;
	bool m_isImplicit;
};

#endif // QUERY_OPERATOR_H
