#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <list>
#include <memory>

#include "utility/math/Vector2.h"
#include "utility/types.h"

class GraphEdge;

class GraphNode
{
public:
	GraphNode(const Id tokenId);
	virtual ~GraphNode();

	virtual std::string getName() const = 0;
	Id getTokenId() const;
	virtual Vec2i getPosition() const = 0;
	virtual void setPosition(const Vec2i& position) = 0;

	virtual bool addOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
	virtual bool addInEdge(const std::weak_ptr<GraphEdge>& edge) = 0;

	virtual void removeOutEdge(GraphEdge* edge) = 0;

	virtual std::list<std::shared_ptr<GraphNode> > getSubNodes() const = 0;
	virtual void addSubNode(const std::shared_ptr<GraphNode>& node) = 0;

	virtual void notifyParentMoved() = 0;

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
		, subNodes()
		, actualNode()
	{
	}

	bool operator==(const DummyNode& other) const
	{
		if (tokenId == other.tokenId
			&& name == other.name
			&& position == position)
		{
			return true;
		}
		return false;
	}

	bool operator!=(const DummyNode& other) const
	{
		return !(*this == other);
	}

	bool operator<(const DummyNode& other) const
	{
		if (tokenId < other.tokenId)
		{
			return true;
		}
		return false;
	}

	bool operator>(const DummyNode& other) const
	{
		return !(*this < other);
	}

	DummyNode& operator=(const DummyNode& other)
	{
		name = other.name;
		tokenId = other.tokenId;
		position = other.position;
		subNodes = other.subNodes;
		actualNode = other.actualNode;
		return *this;
	}

	std::string name;
	Id tokenId;
	Vec2i position;

	std::vector<DummyNode> subNodes;

	std::weak_ptr<GraphNode> actualNode;
};

#endif // GRAPH_NODE_H
