#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <memory>

#include "utility/math/Vector2.h"
#include "utility/types.h"

class GraphEdge;

class GraphNode
{
public:
	GraphNode(const Id tokenId);
	virtual ~GraphNode();

	virtual std::string getName() = 0;
	Id getTokenId();
	virtual Vec2i getPosition() = 0;

	virtual void addOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
	virtual void addInEdge(const std::weak_ptr<GraphEdge>& edge) = 0;

	virtual void removeOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;

protected:
	Id m_tokenId;
};

struct DummyNode
{
public:
	DummyNode(const std::string& n, const Id tId, const Vec2i& p)
		: name(n)
		, tokenId(tId)
		, position(p)
	{
	}

	std::string name;
	Id tokenId;
	Vec2i position;

	std::weak_ptr<GraphNode> actualNode;
};

#endif // GRAPH_NODE_H
