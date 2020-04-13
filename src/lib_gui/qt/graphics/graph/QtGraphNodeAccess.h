#ifndef QT_GRAPH_NODE_ACCESS_H
#define QT_GRAPH_NODE_ACCESS_H

#include "QtGraphNode.h"

#include "../../../../lib/data/parser/AccessKind.h"

class QtGraphNodeAccess: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeAccess(AccessKind accessKind);
	virtual ~QtGraphNodeAccess();

	AccessKind getAccessKind() const;

	// QtGraphNode implementation
	virtual bool isAccessNode() const override;

	virtual void addSubNode(QtGraphNode* node) override;
	virtual void updateStyle() override;

	void hideLabel();

protected:
	virtual void matchName(const std::wstring& query, std::vector<QtGraphNode*>* matchedNodes) override
	{
	}

private:
	AccessKind m_accessKind;

	QGraphicsPixmapItem* m_accessIcon;
	int m_accessIconSize;
};

#endif	  // QT_GRAPH_NODE_ACCESS_H
