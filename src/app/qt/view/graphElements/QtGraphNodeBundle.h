#ifndef QT_GRAPH_NODE_BUNDLE_H
#define QT_GRAPH_NODE_BUNDLE_H

#include "qt/view/graphElements/QtGraphNode.h"

class QtCountCircleItem;

class QtGraphNodeBundle
	: public QtGraphNode
{
public:
	QtGraphNodeBundle(Id tokenId, size_t nodeCount, std::string name);
	virtual ~QtGraphNodeBundle();

	// QtGraphNode implementation
	virtual bool isBundleNode() const;
	virtual Id getTokenId() const;

	virtual void onClick();

	virtual void updateStyle();

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	QtCountCircleItem* m_circle;
	Id m_tokenId;
};

#endif // QT_GRAPH_NODE_BUNDLE_H
