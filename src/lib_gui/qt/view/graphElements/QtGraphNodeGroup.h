#ifndef QT_GRAPH_NODE_GROUP_H
#define QT_GRAPH_NODE_GROUP_H

#include "data/GroupType.h"
#include "qt/view/graphElements/QtGraphNode.h"

class QGraphicsPolygonItem;

class QtGraphNodeGroup
	: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeGroup(Id tokenId, const std::wstring& name, GroupType type, bool interactive);
	virtual ~QtGraphNodeGroup();

	// QtGraphNode implementation
	virtual bool isGroupNode() const;
	virtual Id getTokenId() const;

	virtual void onClick();
	virtual void updateStyle();

protected:
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

private:
	Id m_tokenId;
	GroupType m_type;
	const bool m_interactive;

	QGraphicsPolygonItem* m_background = nullptr;
};

#endif // QT_GRAPH_NODE_GROUP_H
