#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include "qgraphicsitem.h"

#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/math/Vector2.h"

#include "component/view/graphElements/GraphNode.h"

class QtGraphEdge;

class QtGraphNode:
	public GraphNode,
	public QGraphicsRectItem
{
public:
	QtGraphNode(const Vec2i& position, const std::string& name, const Id tokenId);
	virtual ~QtGraphNode();

	virtual std::string getName();
	virtual Vec2i getPosition();

	virtual void addOutEdge(const std::shared_ptr<GraphEdge>& edge);
	virtual void addInEdge(const std::weak_ptr<GraphEdge>& edge);

	virtual void removeOutEdge(const std::shared_ptr<GraphEdge>& edge);

	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
	QGraphicsTextItem* m_text;

	std::list<std::shared_ptr<GraphEdge> > m_outEdges;
	std::list<std::weak_ptr<GraphEdge> > m_inEdges;

	Vec2i m_mouseOffset;
};

#endif // QT_GRAPH_NODE_H
