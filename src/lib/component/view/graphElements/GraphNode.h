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

	virtual bool isAccessNode() const = 0;

	virtual Vec2i getPosition() const = 0;
	virtual bool setPosition(const Vec2i& position) = 0;

	virtual Vec2i getSize() const = 0;
	virtual void setSize(const Vec2i& size) = 0;

	virtual Vec4i getBoundingRect() const = 0;
	virtual Vec4i getParentBoundingRect() const = 0;

	virtual void addOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
	virtual void addInEdge(const std::weak_ptr<GraphEdge>& edge) = 0;

	virtual size_t getOutEdgeCount() const = 0;
	virtual size_t getInEdgeCount() const = 0;

protected:
	const Node* m_data;
};

struct DummyNode
{
public:
	DummyNode()
		: visible(false)
		, childVisible(false)
		, topLevelAncestorId(0)
		, tokenId(0)
		, data(nullptr)
		, active(false)
		, connected(false)
		, aggregated(false)
		, expanded(false)
		, autoExpanded(false)
		, accessType(TokenComponentAccess::ACCESS_NONE)
		, invisibleSubNodeCount(0)
	{
	}

	bool isGraphNode() const
	{
		return data != nullptr;
	}

	bool isAccessNode() const
	{
		return accessType != TokenComponentAccess::ACCESS_NONE;
	}

	bool isExpandToggleNode() const
	{
		return !data && !isAccessNode();
	}

	bool isExpanded() const
	{
		return expanded || autoExpanded;
	}

	Vec2i position;
	Vec2i size;

	bool visible;
	bool childVisible;

	Id topLevelAncestorId;
	Id tokenId;

	std::vector<DummyNode> subNodes;

	// GraphNode
	const Node* data;
	bool active;
	bool connected;
	bool aggregated;
	bool expanded;
	bool autoExpanded;

	// AccessNode
	TokenComponentAccess::AccessType accessType;

	// ExpandToggleNode
	size_t invisibleSubNodeCount;
};

#endif // GRAPH_NODE_H
