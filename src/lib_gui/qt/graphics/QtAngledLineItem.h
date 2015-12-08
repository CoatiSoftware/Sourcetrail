#ifndef QT_ANGLED_LINE_ITEM_H
#define QT_ANGLED_LINE_ITEM_H

#include <QGraphicsItem>

#include "utility/math/Vector4.h"

#include "component/view/GraphViewStyle.h"

class QtAngledLineItem
	: public QGraphicsLineItem
{
public:
	QtAngledLineItem(QGraphicsItem* parent);
	virtual ~QtAngledLineItem();

	void updateLine(
		Vec4i ownerRect, Vec4i targetRect,
		Vec4i ownerParentRect, Vec4i targetParentRect,
		GraphViewStyle::EdgeStyle style);

	void setOnBack(bool back);
	void setHorizontalIn(bool horizontal);

	virtual QPainterPath shape() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* options, QWidget* widget);

private:
	QPolygon getPath() const;
	int getDirection(const QPointF& a, const QPointF& b) const;

	Vec4i m_ownerRect;
	Vec4i m_targetRect;

	Vec4i m_ownerParentRect;
	Vec4i m_targetParentRect;

	GraphViewStyle::EdgeStyle m_style;

	bool m_onBack;
	bool m_horizontalIn;
};

#endif // QT_ANGLED_LINE_ITEM_H
