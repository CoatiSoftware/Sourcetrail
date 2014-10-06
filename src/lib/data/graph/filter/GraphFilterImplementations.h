#ifndef GRAPH_FILTER_IMPLEMENTATIONS_H
#define GRAPH_FILTER_IMPLEMENTATIONS_H

#include <set>

#include "data/graph/Edge.h"
#include "data/graph/filter/GraphFilter.h"
#include "data/graph/FilterableGraph.h"
#include "data/graph/Node.h"
#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentDataType.h"
#include "data/graph/token_component/TokenComponentStatic.h"

/*
 * empty GraphFilterImplementation for copy-pasting
 *

class GraphFilter: public GraphFilter
{
protected:
	virtual void visitNode(Node* node)
	{
	}
};
*/

class GraphFilterCommandMember
	: public GraphFilter
{
protected:
	virtual void visitNode(Node* node)
	{
		node->forEachChildNode(
			[this](Node* n)
			{
				addNode(n);
			}
		);
	}
};

class GraphFilterCommandParent
	: public GraphFilter
{
protected:
	virtual void visitNode(Node* node)
	{
		Node* parent = node->getParentNode();
		if (parent)
		{
			addNode(parent);
		}
	}
};

class GraphFilterCommandNodeType
	: public GraphFilter
{
public:
	GraphFilterCommandNodeType(Node::NodeTypeMask mask)
		: m_mask(mask)
	{
	}

protected:
	virtual void visitNode(Node* node)
	{
		if (node->isType(m_mask))
		{
			addNode(node);
		}
	}

private:
	const Node::NodeTypeMask m_mask;
};

class GraphFilterCommandConst
	: public GraphFilter
{
protected:
	virtual void visitNode(Node* node)
	{
		if (node->getType() == Node::NODE_METHOD && node->getComponent<TokenComponentConst>())
		{
			addNode(node);
		}

		Edge* edge = node->findEdgeOfType(Edge::EDGE_TYPE_OF);
		if (!edge)
		{
			return;
		}

		TokenComponentDataType* type = edge->getComponent<TokenComponentDataType>();
		if (type && type->isConstQualified())
		{
			addNode(node);
		}
	}
};

class GraphFilterCommandStatic
	: public GraphFilter
{
protected:
	virtual void visitNode(Node* node)
	{
		if (node->getComponent<TokenComponentStatic>())
		{
			addNode(node);
		}
	}
};

class GraphFilterCommandAccessType
	: public GraphFilter
{
public:
	GraphFilterCommandAccessType(TokenComponentAccess::AccessType type)
		: m_type(type)
	{
	}

protected:
	virtual void visitNode(Node* node)
	{
		Edge* edge = node->getMemberEdge();
		if (!edge)
		{
			return;
		}

		TokenComponentAccess* access = edge->getComponent<TokenComponentAccess>();
		if (access && access->getAccess() == m_type)
		{
			addNode(edge->getTo());
		}
	}

private:
	const TokenComponentAccess::AccessType m_type;
};

class GraphFilterCommandAbstractionType
	: public GraphFilter
{
public:
	GraphFilterCommandAbstractionType(TokenComponentAbstraction::AbstractionType type)
		: m_type(type)
	{
	}

protected:
	virtual void visitNode(Node* node)
	{
		TokenComponentAbstraction* abstraction = node->getComponent<TokenComponentAbstraction>();
		if (abstraction && abstraction->getAbstraction() == m_type)
		{
			addNode(node);
		}
	}

private:
	const TokenComponentAbstraction::AbstractionType m_type;
};

class GraphFilterCommandCall
	: public GraphFilter
{
public:
	GraphFilterCommandCall(bool callers)
		: m_callers(callers)
	{
	}

protected:
	virtual void visitNode(Node* node)
	{
		node->forEachEdgeOfType(Edge::EDGE_CALL,
			[this, node](Edge* edge)
			{
				Node* from = edge->getFrom();
				Node* to = edge->getTo();

				if (m_callers && node == to)
				{
					addNode(from);
				}
				else if (!m_callers && node == from)
				{
					addNode(to);
				}
			}
		);
	}

private:
	const bool m_callers;
};

class GraphFilterCommandUsage
	: public GraphFilter
{
protected:
	virtual void visitNode(Node* node)
	{
		Edge::EdgeTypeMask mask;
		mask = Edge::EDGE_TYPE_OF | Edge::EDGE_RETURN_TYPE_OF | Edge::EDGE_PARAMETER_TYPE_OF;
		mask = Edge::EDGE_TYPE_USAGE | Edge::EDGE_USAGE | Edge::EDGE_TYPEDEF_OF | mask;

		node->forEachEdge(
			[this, node, mask](Edge* edge)
			{
				if (node == edge->getTo() && edge->isType(mask))
				{
					addNode(edge->getFrom());
				}
			}
		);
	}
};

class GraphFilterCommandInheritance
	: public GraphFilter
{
public:
	GraphFilterCommandInheritance(bool super)
		: m_super(super)
	{
	}

protected:
	virtual void visitNode(Node* node)
	{
		node->forEachEdgeOfType(Edge::EDGE_INHERITANCE,
			[this, node](Edge* edge)
			{
				Node* from = edge->getFrom();
				Node* to = edge->getTo();

				if (m_super && node == from)
				{
					addNode(to);
				}
				else if (!m_super && node == to)
				{
					addNode(from);
				}
			}
		);
	}

private:
	const bool m_super;
};

class GraphFilterToken
	: public GraphFilter
{
public:
	GraphFilterToken(const std::string& tokenName, const std::set<Id>& tokenIds)
		: m_tokenName(tokenName)
		, m_tokenIds(tokenIds)
	{
	}

	virtual void apply(const FilterableGraph* in, FilterableGraph* out)
	{
		if (m_tokenIds.size())
		{
			for (Id tokenId : m_tokenIds)
			{
				Node* node = in->getNodeById(tokenId);
				if (node)
				{
					out->addNode(node);
				}
			}
		}
		else
		{
			GraphFilter::apply(in, out);
		}
	}

protected:
	virtual void visitNode(Node* node)
	{
		if (node->getFullName() == m_tokenName)
		{
			addNode(node);
		}
	}

private:
	const std::string m_tokenName;
	const std::set<Id> m_tokenIds;
};

#endif // GRAPH_FILTER_IMPLEMENTATIONS_H
