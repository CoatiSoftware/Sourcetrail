#include "data/graph/filter/GraphFilterConductor.h"

#include "data/graph/filter/GraphFilter.h"
#include "data/graph/filter/GraphFilterImplementations.h"
#include "data/graph/SubGraph.h"
#include "data/query/QueryCommand.h"
#include "data/query/QueryNode.h"
#include "data/query/QueryOperator.h"
#include "data/query/QueryToken.h"
#include "data/query/QueryTree.h"
#include "utility/logging/logging.h"

GraphFilterConductor::GraphFilterConductor()
{
}

GraphFilterConductor::~GraphFilterConductor()
{
}

void GraphFilterConductor::filter(const QueryTree* tree, const FilterableGraph* in, FilterableGraph* out) const
{
	if (tree->isValid())
	{
		filterRecursively(tree->getRoot().get(), in, out);
	}
}

void GraphFilterConductor::filterRecursively(const QueryNode* node, const FilterableGraph* in, FilterableGraph* out) const
{
	if (node->isOperator())
	{
		filterOperatorNode(dynamic_cast<const QueryOperator*>(node), in, out);
	}
	else if (node->isCommand())
	{
		filterCommandNode(dynamic_cast<const QueryCommand*>(node), in, out);
	}
	else if (node->isToken())
	{
		filterTokenNode(dynamic_cast<const QueryToken*>(node), in, out);
	}
}

void GraphFilterConductor::filterOperatorNode(const QueryOperator* node, const FilterableGraph* in, FilterableGraph* out) const
{
	switch (node->getType())
	{
	case QueryOperator::OPERATOR_NOT:
		{
			SubGraph sub, sub2;
			filterRecursively(node->getRight().get(), in, &sub);

			sub2.copy(in);
			sub2.subtract(sub);
			out->add(&sub2);
		}
		break;

	case QueryOperator::OPERATOR_SUB:
	case QueryOperator::OPERATOR_AND:
		{
			SubGraph sub;
			filterRecursively(node->getLeft().get(), in, &sub);
			filterRecursively(node->getRight().get(), &sub, out);
		}
		break;

	case QueryOperator::OPERATOR_HAS:
		{
			SubGraph sub, sub2;
			filterRecursively(node->getLeft().get(), in, &sub);
			GraphFilterCommandMember().apply(&sub, &sub2);
			filterRecursively(node->getRight().get(), &sub2, out);
		}
		break;

	case QueryOperator::OPERATOR_OR:
		filterRecursively(node->getLeft().get(), in, out);
		filterRecursively(node->getRight().get(), in, out);
		break;

	default:
		break;
	}
}

void GraphFilterConductor::filterCommandNode(const QueryCommand* node, const FilterableGraph* in, FilterableGraph* out) const
{
	switch (node->getType())
	{
	case QueryCommand::COMMAND_MEMBER:
		GraphFilterCommandMember().apply(in, out);
		break;
	case QueryCommand::COMMAND_PARENT:
		GraphFilterCommandParent().apply(in, out);
		break;
	case QueryCommand::COMMAND_FUNCTION:
		GraphFilterCommandNodeType(Node::NODE_FUNCTION).apply(in, out);
		break;
	case QueryCommand::COMMAND_GLOBAL_VARIABLE:
		GraphFilterCommandNodeType(Node::NODE_GLOBAL_VARIABLE).apply(in, out);
		break;
	case QueryCommand::COMMAND_CLASS:
		GraphFilterCommandNodeType(Node::NODE_CLASS).apply(in, out);
		break;
	case QueryCommand::COMMAND_METHOD:
		GraphFilterCommandNodeType(Node::NODE_METHOD).apply(in, out);
		break;
	case QueryCommand::COMMAND_FIELD:
		GraphFilterCommandNodeType(Node::NODE_FIELD).apply(in, out);
		break;
	case QueryCommand::COMMAND_NAMESPACE:
		GraphFilterCommandNodeType(Node::NODE_NAMESPACE).apply(in, out);
		break;
	case QueryCommand::COMMAND_STRUCT:
		GraphFilterCommandNodeType(Node::NODE_STRUCT).apply(in, out);
		break;
	case QueryCommand::COMMAND_ENUM:
		GraphFilterCommandNodeType(Node::NODE_ENUM).apply(in, out);
		break;
	case QueryCommand::COMMAND_TYPEDEF:
		GraphFilterCommandNodeType(Node::NODE_TYPEDEF).apply(in, out);
		break;

	case QueryCommand::COMMAND_CONST:
		GraphFilterCommandConst().apply(in, out);
		break;
	case QueryCommand::COMMAND_STATIC:
		GraphFilterCommandStatic().apply(in, out);
		break;

	case QueryCommand::COMMAND_VIRTUAL:
		GraphFilterCommandAbstractionType(TokenComponentAbstraction::ABSTRACTION_VIRTUAL).apply(in, out);
		break;
	case QueryCommand::COMMAND_PURE_VIRTUAL:
		GraphFilterCommandAbstractionType(TokenComponentAbstraction::ABSTRACTION_PURE_VIRTUAL).apply(in, out);
		break;

	case QueryCommand::COMMAND_PUBLIC:
		GraphFilterCommandAccessType(TokenComponentAccess::ACCESS_PUBLIC).apply(in, out);
		break;
	case QueryCommand::COMMAND_PROTECTED:
		GraphFilterCommandAccessType(TokenComponentAccess::ACCESS_PROTECTED).apply(in, out);
		break;
	case QueryCommand::COMMAND_PRIVATE:
		GraphFilterCommandAccessType(TokenComponentAccess::ACCESS_PRIVATE).apply(in, out);
		break;

	case QueryCommand::COMMAND_CALLER:
		GraphFilterCommandCall(true).apply(in, out);
		break;
	case QueryCommand::COMMAND_CALLEE:
		GraphFilterCommandCall(false).apply(in, out);
		break;

	case QueryCommand::COMMAND_USAGE:
		GraphFilterCommandUsage().apply(in, out);
		break;

	case QueryCommand::COMMAND_SUPER_CLASS:
		GraphFilterCommandInheritance(true).apply(in, out);
		break;
	case QueryCommand::COMMAND_SUB_CLASS:
		GraphFilterCommandInheritance(false).apply(in, out);
		break;

	default:
		LOG_ERROR_STREAM(<< "QueryCommand not supported: " << node->getType());
		GraphFilter().apply(in, out);
		break;
	}
}

void GraphFilterConductor::filterTokenNode(const QueryToken* node, const FilterableGraph* in, FilterableGraph* out) const
{
	GraphFilterToken(node->getTokenName(), node->getTokenIds()).apply(in, out);
}
