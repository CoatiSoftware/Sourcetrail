#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <QGraphicsItem>

#include "utility/math/Vector2.h"
#include "utility/messaging/type/MessageActivateToken.h"

#include "component/view/graphElements/GraphNode.h"

class QtGraphEdge;
class QtGraphNodeComponent;

class QtGraphNode
	: public GraphNode
	, public QGraphicsRectItem
{
public:
	QtGraphNode(const Node* data, TokenComponentAccess::AccessType accessType);
	virtual ~QtGraphNode();

	virtual std::string getName() const;
	virtual Vec2i getPosition() const;
	virtual void setPosition(const Vec2i& position);

	virtual Vec2i getSize() const;

	bool getIsActive() const;
	void setIsActive(bool isActive);

	QtGraphNode* getParent() const;
	void setParent(std::weak_ptr<QtGraphNode> parentNode);

	void addComponent(const std::shared_ptr<QtGraphNodeComponent>& component);

	virtual bool addOutEdge(const std::shared_ptr<GraphEdge>& edge);
	virtual bool addInEdge(const std::weak_ptr<GraphEdge>& edge);

	virtual void removeOutEdge(GraphEdge* edge);

	virtual std::list<std::shared_ptr<GraphNode>> getSubNodes() const;
	virtual void addSubNode(const std::shared_ptr<GraphNode>& node);

	virtual void notifyParentMoved();

	virtual void hideContent();
	virtual void showContent();

	virtual void hide();
	virtual void show();

	virtual bool isHidden();
	virtual bool contentIsHidden();

	virtual unsigned int getOutEdgeCount() const;
	virtual unsigned int getInEdgeCount() const;

	/**
	 * @brief Returns the count of all connected edges and active nodes (including sub nodes)
	 */
	virtual unsigned int getEdgeAndActiveCountRecursive() const;

	void onClick();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	void notifyEdgesAfterMove();
	void notifyParentNodeAfterSizeChanged();

	void onChildSizeChanged();

	std::list<std::shared_ptr<GraphEdge>> m_outEdges;
	std::list<std::weak_ptr<GraphEdge>> m_inEdges;

private:
	void rebuildLayout();

	QGraphicsTextItem* m_text;

	std::weak_ptr<QtGraphNode> m_parentNode;
	std::list<std::shared_ptr<GraphNode>> m_subNodes;

	std::list<std::shared_ptr<QtGraphNodeComponent>> m_components;

	bool m_isActive;

	Vec2i m_baseSize;
	Vec2i m_currentSize;

	Vec2i m_padding;

	bool m_contentHidden;
};

#endif // QT_GRAPH_NODE_H
