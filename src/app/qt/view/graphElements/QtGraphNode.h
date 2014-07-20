#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <QGraphicsItem>

#include "component/view/graphElements/GraphNode.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/math/Vector2.h"

class QtGraphEdge;

class QtGraphNode
	: public GraphNode
	, public QGraphicsRectItem
{
public:
	QtGraphNode(const Vec2i& position, const std::string& name, const Id tokenId);
	virtual ~QtGraphNode();

	virtual std::string getName() const;
	virtual Vec2i getPosition() const;
	virtual void setPosition(const Vec2i& position);

	virtual bool addOutEdge(const std::shared_ptr<GraphEdge>& edge);
	virtual bool addInEdge(const std::weak_ptr<GraphEdge>& edge);

	virtual void removeOutEdge(GraphEdge* edge);

	virtual std::list<std::shared_ptr<GraphNode> > getSubNodes() const;
	virtual void addSubNode(const std::shared_ptr<GraphNode>& node);

	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

	virtual void notifyParentMoved();

protected:
	void notifyEdgesAfterMove();

	std::list<std::shared_ptr<GraphEdge> > m_outEdges;
	std::list<std::weak_ptr<GraphEdge> > m_inEdges;

private:
	QGraphicsTextItem* m_text;

	std::list<std::shared_ptr<GraphNode> > m_subNodes;
};

#endif // QT_GRAPH_NODE_H
