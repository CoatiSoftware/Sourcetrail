#ifndef QT_GRAPH_NODE_GROUP_H
#define QT_GRAPH_NODE_GROUP_H

#include "data/NodeType.h"
#include "qt/view/graphElements/QtGraphNode.h"

class QGraphicsRectItem;
class QtRoundedRectItem;

class QtGraphNodeGroup
	: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeGroup(Id tokenId, const std::wstring& name, NodeType::GroupType type);
	virtual ~QtGraphNodeGroup();

	// QtGraphNode implementation
	virtual bool isGroupNode() const;
	virtual Id getTokenId() const;

	virtual void onClick();
	virtual void updateStyle();

private:
	Id m_tokenId;
	NodeType::GroupType m_type;

	QtRoundedRectItem* m_background = nullptr;
	QGraphicsRectItem* m_backgroundTopRight = nullptr;
	QGraphicsRectItem* m_backgroundBottomLeft = nullptr;
};

#endif // QT_GRAPH_NODE_GROUP_H
