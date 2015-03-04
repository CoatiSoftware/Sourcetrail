#include "data/query/QueryTree.h"

#include "data/query/QueryCommand.h"
#include "data/query/QueryNode.h"
#include "data/query/QueryOperator.h"
#include "data/query/QueryToken.h"
#include "utility/utilityString.h"

std::deque<std::string> QueryTree::tokenizeQuery(const std::string& query)
{
	std::deque<std::string> tokensTmp = utility::split(query, QueryOperator::getOperator(QueryOperator::OPERATOR_NONE));

	for (const std::pair<char, QueryOperator::OperatorType>& p : QueryOperator::getOperatorTypeMap())
	{
		tokensTmp = utility::tokenize(tokensTmp, p.first);
	}

	char operatorToken = QueryOperator::getOperator(QueryOperator::OPERATOR_TOKEN);
	char operatorCommand = QueryOperator::getOperator(QueryOperator::OPERATOR_COMMAND);

	bool isToken = false;
	bool isCommand = false;

	std::string token;
	std::deque<std::string> tokens;

	while (tokensTmp.size())
	{
		std::string tokenTmp = tokensTmp.front();
		tokensTmp.pop_front();

		token += tokenTmp;

		if (tokenTmp.size() == 1)
		{
			if (tokenTmp[0] == operatorToken && !isCommand)
			{
				isToken = !isToken;
			}
			else if (tokenTmp[0] == operatorCommand && !isToken)
			{
				isCommand = !isCommand;
			}
		}

		if ((!isToken && !isCommand) || (!tokensTmp.size() && token.size()))
		{
			tokens.push_back(token);
			token.clear();
		}
	}

	return tokens;
}

std::string QueryTree::getTokenName(const std::string& token)
{
	if (token.size() > 2 && token.front() == QueryToken::BOUNDARY && token.back() == QueryToken::BOUNDARY)
	{
		return QueryToken(token).getName();
	}
	else if (token.size() > 2 && token.front() == QueryCommand::BOUNDARY && token.back() == QueryCommand::BOUNDARY)
	{
		return QueryCommand(token).getName();
	}

	return token;
}

std::string QueryTree::getTokenTypeName(const QueryNode::QueryNodeType& type)
{
	return QueryNode::queryNodeTypeToString(type);
}

QueryNode::QueryNodeType QueryTree::getTokenType(const std::string& token)
{
	if (token.size() > 2 && token.front() == QueryToken::BOUNDARY && token.back() == QueryToken::BOUNDARY)
	{
		return QueryNode::QUERYNODETYPE_TOKEN;
	}
	else if (token.size() > 2 && token.front() == QueryCommand::BOUNDARY && token.back() == QueryCommand::BOUNDARY)
	{
		return QueryNode::QUERYNODETYPE_COMMAND;
	}
	else if (token.size() == 1 && QueryOperator::getOperatorType(token.front()) != QueryOperator::OPERATOR_NONE)
	{
		return QueryNode::QUERYNODETYPE_OPERATOR;
	}

	return QueryNode::QUERYNODETYPE_NONE;
}

QueryTree::QueryTree()
	: m_valid(true)
{
}

QueryTree::QueryTree(const std::string& query)
	: m_valid(true)
{
	build(query);
}

QueryTree::~QueryTree()
{
}

std::shared_ptr<QueryNode> QueryTree::getRoot() const
{
	return m_root;
}

bool QueryTree::isValid() const
{
	return m_valid;
}

void QueryTree::build(const std::string& query)
{
	std::deque<std::string> tokens = tokenizeQuery(query);

	m_query = utility::join(tokens, ' ');

	m_valid = true;
	m_root = buildTree(tokens, nullptr);
}

void QueryTree::print(std::ostream& ostream) const
{
	ostream << m_query;

	if (!m_valid)
	{
		ostream << " INVALID";
	}

	ostream << '\n';

	if (m_root)
	{
		m_root->print(ostream, 0);
	}
}

std::shared_ptr<QueryNode> QueryTree::buildTree(std::deque<std::string>& tokens, std::shared_ptr<QueryNode> frontNode)
{
	std::shared_ptr<QueryNode> node = getNextNode(tokens);
	std::shared_ptr<QueryOperator> operatorNode = std::dynamic_pointer_cast<QueryOperator>(node);

	if (!node)
	{
		m_valid = false;
		return nullptr;
	}

	if (frontNode)
	{
		if (node->isComplete())
		{
			std::shared_ptr<QueryOperator> subNode = std::make_shared<QueryOperator>(QueryOperator::OPERATOR_SUB, true);
			subNode->setLeft(frontNode);
			subNode->setRight(node);
			node = subNode;
		}
		else if (node->isOperator() && operatorNode)
		{
			operatorNode->setLeft(frontNode);
		}
		else
		{
			m_valid = false;
		}
	}

	if (tokens.size())
	{
		if (node->isComplete())
		{
			node = buildTree(tokens, node);
		}
		else if (node->isOperator() && operatorNode)
		{
			std::shared_ptr<QueryNode> rightNode = buildTree(tokens, nullptr);
			std::shared_ptr<QueryOperator> rightOperatorNode = std::dynamic_pointer_cast<QueryOperator>(rightNode);

			if (!rightNode)
			{
				m_valid = false;
			}
			else if (rightNode->isOperator() && !rightNode->isGroup() &&
				operatorNode->lowerPrecedence(*rightOperatorNode.get()))
			{
				operatorNode->setRight(rightOperatorNode->getLeft());
				rightOperatorNode->setLeft(operatorNode);
				node = rightNode;
			}
			else
			{
				operatorNode->setRight(rightNode);
			}
		}
		else
		{
			m_valid = false;
		}
	}

	if (!node || !node->isComplete())
	{
		m_valid = false;
	}

	return node;
}

std::shared_ptr<QueryNode> QueryTree::buildGroup(std::deque<std::string>& tokens, QueryOperator::OperatorType closeType)
{
	std::deque<std::string> group;
	std::string name;
	char delimiter = QueryOperator::getOperator(closeType);
	bool valid = true;

	while (tokens.size() > 0 && tokens.front() != std::string(1, delimiter))
	{
		group.push_back(tokens.front());
		name += tokens.front();
		tokens.pop_front();
	}

	if (tokens.size())
	{
		tokens.pop_front();
	}
	else
	{
		valid = false;
		m_valid = false;
	}

	if (!group.size())
	{
		return nullptr;
	}

	std::shared_ptr<QueryNode> groupNode = buildTree(group, nullptr);
	groupNode->setIsGroup(true);
	groupNode->setIsComplete(valid);

	return groupNode;
}

std::shared_ptr<QueryNode> QueryTree::getNextNode(std::deque<std::string>& tokens)
{
	while (tokens.size())
	{
		std::string token = tokens.front();
		tokens.pop_front();

		QueryOperator::OperatorType type = QueryOperator::getOperatorType(token[0]);

		switch (type)
		{
		case QueryOperator::OPERATOR_NOT:
		case QueryOperator::OPERATOR_SUB:
		case QueryOperator::OPERATOR_HAS:
		case QueryOperator::OPERATOR_AND:
		case QueryOperator::OPERATOR_OR:
			return std::make_shared<QueryOperator>(type, false);

		case QueryOperator::OPERATOR_TOKEN:
			return createToken(token);

		case QueryOperator::OPERATOR_COMMAND:
			return createCommand(token);

		case QueryOperator::OPERATOR_GROUP_OPEN:
			return buildGroup(tokens, QueryOperator::OPERATOR_GROUP_CLOSE);
		case QueryOperator::OPERATOR_GROUP_CLOSE:
			m_valid = false;
			break;

		case QueryOperator::OPERATOR_NONE:
			return createToken(token);
		}
	}

	return nullptr;
}

std::shared_ptr<QueryNode> QueryTree::createCommand(const std::string& name)
{
	if (name.size() < 3 || name.front() != QueryCommand::BOUNDARY || name.back() != QueryCommand::BOUNDARY)
	{
		m_valid = false;
	}

	std::shared_ptr<QueryCommand> node = std::make_shared<QueryCommand>(name);

	if (node->getType() == QueryCommand::COMMAND_INVALID)
	{
		m_valid = false;
	}

	return node;
}

std::shared_ptr<QueryNode> QueryTree::createToken(const std::string& name)
{
	if (name.size() < 3 || name.front() != QueryToken::BOUNDARY || name.back() != QueryToken::BOUNDARY)
	{
		m_valid = false;
	}

	return std::make_shared<QueryToken>(name);
}

std::ostream& operator<<(std::ostream& ostream, const QueryTree& tree)
{
	tree.print(ostream);
	return ostream;
}
