#ifndef QT_GRAPH_NODE_BUNDLE_H
#define QT_GRAPH_NODE_BUNDLE_H

#include "Node.h"
#include "QtGraphNode.h"

class QtCountCircleItem;

class QtGraphNodeBundle: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeBundle(Id tokenId, size_t nodeCount, NodeType type, const std::wstring& name);
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
	NodeType m_type;
};

#endif	  // QT_GRAPH_NODE_BUNDLE_H
