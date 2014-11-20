#ifndef QT_GRAPH_NODE_ACCESS_H
#define QT_GRAPH_NODE_ACCESS_H

#include "data/graph/token_component/TokenComponentAccess.h"
#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeAccess
	: public QtGraphNode
{
public:
	class QtAccessToggle
		: public QGraphicsRectItem
	{
	public:
		QtAccessToggle(bool expanded, int invisibleSubNodeCount, QGraphicsItem* parent);
		virtual ~QtAccessToggle();

	private:
		QGraphicsPixmapItem* m_icon;
		QGraphicsSimpleTextItem* m_number;
	};


	QtGraphNodeAccess(TokenComponentAccess::AccessType accessType, bool expanded, int invisibleSubNodeCount);
	virtual ~QtGraphNodeAccess();

	virtual void setSize(const Vec2i& size);

	virtual void addSubNode(const std::shared_ptr<QtGraphNode>& node);

	virtual void onClick();

private:
	TokenComponentAccess::AccessType m_access;

	QGraphicsPixmapItem* m_accessIcon;
	int m_accessIconSize;

	QtAccessToggle* m_accessToggle;
};

#endif // QT_GRAPH_NODE_ACCESS_H
