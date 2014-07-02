#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include "qgraphicsitem.h"

#include "utility/math/Vector2.h"

#include "component/view/graphElements/GraphNode.h"

class QtGraphNode:
	public GraphNode,
	public QGraphicsRectItem
{
public:
	QtGraphNode(const Vec2i& position, const std::string& name);
	virtual ~QtGraphNode();

private:
	QGraphicsTextItem* m_text;
};

#endif // QT_GRAPH_NODE_H
