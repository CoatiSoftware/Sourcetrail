#ifndef QT_EXPAND_TOGGLE_H
#define QT_EXPAND_TOGGLE_H

#include <QGraphicsItem>

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeExpandToggle
	: public QtGraphNode
{
public:
	QtGraphNodeExpandToggle(bool expanded, int invisibleSubNodeCount);
	virtual ~QtGraphNodeExpandToggle();

	virtual bool isExpandToggleNode() const;

	virtual void onClick();
	virtual void updateStyle();

private:
	QGraphicsPixmapItem* m_icon;
	bool m_allVisible;
};

#endif // QT_EXPAND_TOGGLE_H
