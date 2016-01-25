#ifndef QT_GRAPH_NODE_DATA_H
#define QT_GRAPH_NODE_DATA_H

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeData
	: public QtGraphNode
{
public:
	QtGraphNodeData(const Node* data, const std::string& name, bool hasParent, bool childVisible);
	virtual ~QtGraphNodeData();

	const Node* getData() const;

	// QtGraphNode implementation
	virtual bool isDataNode() const;

	virtual Id getTokenId() const;

	virtual void onClick();
	virtual void moved(const Vec2i& oldPosition);
	virtual void updateStyle();

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	const Node* m_data;
	bool m_childVisible;
};

#endif // QT_GRAPH_NODE_DATA_H
