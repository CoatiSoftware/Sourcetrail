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
	DummyNode(const Node* data)
		: data(data)
		, accessType(TokenComponentAccess::ACCESS_NONE)
		, active(false)
		, connected(false)
		, expanded(false)
		, invisibleSubNodeCount(0)
		, visible(false)
	{
	}

	DummyNode(TokenComponentAccess::AccessType accessType)
		: data(nullptr)
		, accessType(accessType)
		, active(false)
		, connected(false)
		, expanded(false)
		, invisibleSubNodeCount(0)
		, visible(false)
	{
	}

	const Node* data;
	TokenComponentAccess::AccessType accessType;

	Vec2i position;
	Vec2i size;

	bool active;
	bool connected;

	bool expanded;
	size_t invisibleSubNodeCount;

	bool visible;

	std::vector<DummyNode> subNodes;
};

#endif // GRAPH_NODE_H
