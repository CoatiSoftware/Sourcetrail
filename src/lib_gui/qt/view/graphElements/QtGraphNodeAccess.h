#ifndef QT_GRAPH_NODE_ACCESS_H
#define QT_GRAPH_NODE_ACCESS_H

#include "qt/view/graphElements/QtGraphNode.h"

#include "data/parser/AccessKind.h"

class QtGraphNodeAccess
	: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeAccess(AccessKind accessKind);
	virtual ~QtGraphNodeAccess();

	AccessKind getAccessKind() const;

	// QtGraphNode implementation
	virtual bool isAccessNode() const;

	virtual void addSubNode(const std::shared_ptr<QtGraphNode>& node);
	virtual void updateStyle();

	void hideLabel();

protected:
	virtual void matchName(const std::string& query, std::vector<QtGraphNode*>* matchedNodes) {}

private:
	AccessKind m_accessKind;

	QGraphicsPixmapItem* m_accessIcon;
	int m_accessIconSize;
};

#endif // QT_GRAPH_NODE_ACCESS_H
