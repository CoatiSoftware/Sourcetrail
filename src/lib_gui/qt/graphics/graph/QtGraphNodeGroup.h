#ifndef QT_GRAPH_NODE_GROUP_H
#define QT_GRAPH_NODE_GROUP_H

#include "GroupType.h"
#include "QtGraphNode.h"

class QGraphicsPolygonItem;
class QPainterPath;

class QtGraphNodeGroup: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeGroup(
		GraphFocusHandler* focusHandler,
		Id tokenId,
		const std::wstring& name,
		GroupType type,
		bool interactive);
	virtual ~QtGraphNodeGroup();

	// QtGraphNode implementation
	virtual bool isGroupNode() const;
	virtual Id getTokenId() const;

	virtual void onClick();
	virtual void updateStyle();

	virtual QPainterPath shape() const;

protected:
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

private:
	Id m_tokenId;
	GroupType m_type;

	QGraphicsPolygonItem* m_background = nullptr;
	mutable QPainterPath m_path;
};

#endif	  // QT_GRAPH_NODE_GROUP_H
