#ifndef QT_GRAPH_NODE_ACCESS_H
#define QT_GRAPH_NODE_ACCESS_H

#include "data/graph/token_component/TokenComponentAccess.h"
#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeAccess
	: public QtGraphNode
{
public:
	class QtAccessArrow
		: public QGraphicsRectItem
	{
	public:
		QtAccessArrow(QGraphicsItem* parent);
		virtual ~QtAccessArrow();

		int update(bool visible, int number);

	private:
		QGraphicsPixmapItem* m_icon;
		QGraphicsSimpleTextItem* m_number;
	};


	QtGraphNodeAccess(TokenComponentAccess::AccessType accessType);
	virtual ~QtGraphNodeAccess();

	virtual void hideContent();
	virtual void showContent();

	virtual void onClick();

	virtual void rebuildLayout();

private:
	bool m_contentHidden;

	QGraphicsPixmapItem* m_accessIcon;
	int m_accessIconSize;

	QtAccessArrow* m_arrow;
};

#endif // QT_GRAPH_NODE_ACCESS_H
