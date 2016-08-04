#ifndef QT_GRAPH_NODE_ACCESS_H
#define QT_GRAPH_NODE_ACCESS_H

#include "data/graph/token_component/TokenComponentAccess.h"
#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeAccess
	: public QtGraphNode
{
public:
	QtGraphNodeAccess(AccessKind accessKind);
	virtual ~QtGraphNodeAccess();

	AccessKind getAccessKind() const;

	// QtGraphNode implementation
	virtual bool isAccessNode() const;

	virtual void addSubNode(const std::shared_ptr<QtGraphNode>& node);
	virtual void updateStyle();

	void hideLabel();

private:
	AccessKind m_accessKind;

	QGraphicsPixmapItem* m_accessIcon;
	int m_accessIconSize;
};

#endif // QT_GRAPH_NODE_ACCESS_H
