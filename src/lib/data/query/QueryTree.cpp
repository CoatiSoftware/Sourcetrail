#include "data/query/QueryTree.h"

#include "data/query/QueryCommand.h"
#include "data/query/QueryNode.h"
#include "data/query/QueryOperator.h"
#include "data/query/QueryToken.h"
#include "utility/utilityString.h"

QueryTree::QueryTree(std::string query)
	: m_valid(true)
{
	std::deque<std::string> tokens =
		utility::split<std::deque<std::string>>(query, QueryOperator::getOperator(QueryOperator::OPERATOR_NONE));

	for (const std::pair<char, QueryOperator::OperatorType>& p : QueryOperator::getOperatorTypeMap())
	{
		tokens = utility::tokenize<std::deque<std::string>>(tokens, p.first);
	}

	for (std::string str : tokens)
	{
		m_query += str + ' ';
	}

	m_root = buildTree(tokens, nullptr);
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

	if (frontNode)
	{
		if (node->isComplete())
		{
			std::shared_ptr<QueryOperator> subNode = std::make_shared<QueryOperator>(QueryOperator::OPERATOR_SUB);
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

			if (rightNode->isOperator() && !rightNode->isGroup() &&
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

	if (!node->isComplete())
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
	while (tokens.front() != std::string(1, delimiter) && tokens.size())
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
		m_valid = false;
	}

	if (!group.size())
	{
		return nullptr;
	}

	if (closeType == QueryOperator::OPERATOR_NAME)
	{
		return std::make_shared<QueryToken>(name);
	}

	std::shared_ptr<QueryNode> groupNode = buildTree(group, nullptr);
	groupNode->setIsGroup(true);

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
			return std::make_shared<QueryOperator>(type);

		case QueryOperator::OPERATOR_NAME:
			return buildGroup(tokens, QueryOperator::OPERATOR_NAME);
		case QueryOperator::OPERATOR_GROUP_OPEN:
			return buildGroup(tokens, QueryOperator::OPERATOR_GROUP_CLOSE);
		case QueryOperator::OPERATOR_GROUP_CLOSE:
			m_valid = false;
			break;

		case QueryOperator::OPERATOR_NONE:
			return createCommand(token);
		}
	}

	return nullptr;
}

std::shared_ptr<QueryNode> QueryTree::createCommand(std::string name)
{
	std::shared_ptr<QueryCommand> node = std::make_shared<QueryCommand>(name);

	if (node->getType() == QueryCommand::COMMAND_INVALID)
	{
		m_valid = false;
	}

	return node;
}

std::ostream& operator<<(std::ostream& ostream, const QueryTree& tree)
{
	tree.print(ostream);
	return ostream;
}
