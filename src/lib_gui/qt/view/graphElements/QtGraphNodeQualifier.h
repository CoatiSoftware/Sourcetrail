#ifndef QT_GRAPH_NODE_QUALIFIER_H
#define QT_GRAPH_NODE_QUALIFIER_H

#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeQualifier
	: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeQualifier(const NameHierarchy& name);
	virtual ~QtGraphNodeQualifier();

	// QtGraphNode implementation
	virtual bool isQualifierNode() const;

	virtual bool setPosition(const Vec2i& pos);

	virtual void onClick();
	virtual void updateStyle();

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	const NameHierarchy m_qualifierName;

	QGraphicsRectItem* m_background;
	QGraphicsRectItem* m_leftBorder;
	QGraphicsPolygonItem* m_rightArrow;
	QGraphicsPolygonItem* m_rightArrowSmall;
	QGraphicsSimpleTextItem* m_name;

	Vec2i m_pos;
};

#endif // QT_GRAPH_NODE_QUALIFIER_H
