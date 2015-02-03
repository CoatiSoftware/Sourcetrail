#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <list>
#include <memory>

#include "utility/math/Vector2.h"
#include "utility/math/Vector4.h"
#include "utility/types.h"

#include "data/graph/Node.h"
#include "data/graph/token_component/TokenComponentAccess.h"

class GraphEdge;

class GraphNode
{
public:
	GraphNode(const Node* data);
	virtual ~GraphNode();

	virtual std::string getName() const = 0;
	Id getTokenId() const;

	const Node* getData() const;
	void setData(const Node* data);

	virtual Vec2i getPosition() const = 0;
	virtual bool setPosition(const Vec2i& position) = 0;
	virtual void moveTo(const Vec2i& position) = 0;

	virtual Vec2i getSize() const = 0;
	virtual void setSize(const Vec2i& size) = 0;

	virtual Vec4i getBoundingRect() const = 0;
	virtual Vec4i getParentBoundingRect() const = 0;

	virtual bool addOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
	virtual bool addInEdge(const std::weak_ptr<GraphEdge>& edge) = 0;

	virtual unsigned int getOutEdgeCount() const = 0;
	virtual unsigned int getInEdgeCount() const = 0;

protected:
	const Node* m_data;
};

struct DummyNode
{
public:
	DummyNode()
		: data(nullptr)
		, accessType(TokenComponentAccess::ACCESS_NONE)
		, active(false)
		, connected(false)
		, aggregated(false)
		, expanded(false)
		, autoExpanded(false)
		, invisibleSubNodeCount(0)
		, visible(false)
		, topLevelAncestorId(0)
		, tokenId(0)
	{
	}

	bool isExpanded() const
	{
		return expanded || autoExpanded;
	}

	const Node* data;
	TokenComponentAccess::AccessType accessType;

	Vec2i position;
	Vec2i size;

	bool active;
	bool connected;
	bool aggregated;

	bool expanded;
	bool autoExpanded;
	size_t invisibleSubNodeCount;

	bool visible;

	Id topLevelAncestorId;
	Id tokenId;

	std::vector<DummyNode> subNodes;
};

#endif // GRAPH_NODE_H
