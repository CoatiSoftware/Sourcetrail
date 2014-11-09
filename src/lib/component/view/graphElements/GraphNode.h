#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <list>
#include <memory>

#include "utility/math/Vector2.h"
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
	virtual void setPosition(const Vec2i& position) = 0;

	virtual Vec2i getSize() const = 0;

	virtual bool addOutEdge(const std::shared_ptr<GraphEdge>& edge) = 0;
	virtual bool addInEdge(const std::weak_ptr<GraphEdge>& edge) = 0;

	virtual void removeOutEdge(GraphEdge* edge) = 0;

	virtual std::list<std::shared_ptr<GraphNode> > getSubNodes() const = 0;
	virtual void addSubNode(const std::shared_ptr<GraphNode>& node) = 0;

	virtual void notifyParentMoved() = 0;

	virtual void hideContent() = 0;
	virtual void showContent() = 0;

	virtual void hide() = 0;
	virtual void show() = 0;

	virtual bool isHidden() = 0;
	virtual bool contentIsHidden() = 0;

	virtual unsigned int getOutEdgeCount() const = 0;
	virtual unsigned int getInEdgeCount() const = 0;

	virtual unsigned int getEdgeAndActiveCountRecursive() const = 0;

protected:
	const Node* m_data;
};

struct DummyNode
{
	DummyNode(const Node* data)
		: data(data)
		, accessType(TokenComponentAccess::ACCESS_NONE)
	{
	}

	DummyNode(TokenComponentAccess::AccessType accessType)
		: data(nullptr)
		, accessType(accessType)
	{
	}

	const Node* data;
	const TokenComponentAccess::AccessType accessType;

	Vec2i position;
	std::vector<DummyNode> subNodes;
};

#endif // GRAPH_NODE_H
