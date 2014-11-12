#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <QGraphicsItem>

#include "utility/math/Vector2.h"
#include "utility/math/Vector4.h"

#include "component/view/graphElements/GraphNode.h"

class QtGraphEdge;
class QtGraphicsRoundedRectItem;
class QtGraphNodeComponent;

class QtGraphNode
	: public GraphNode
	, public QGraphicsRectItem
{
public:
	QtGraphNode();
	QtGraphNode(const Node* data);
	virtual ~QtGraphNode();

	virtual std::string getName() const;
	void setName(const std::string& name);

	virtual Vec2i getPosition() const;
	virtual void setPosition(const Vec2i& position);

	virtual Vec2i getSize() const;
	virtual void setSize(Vec2i size);

	virtual bool addOutEdge(const std::shared_ptr<GraphEdge>& edge);
	virtual bool addInEdge(const std::weak_ptr<GraphEdge>& edge);

	virtual void removeOutEdge(GraphEdge* edge);

	virtual unsigned int getOutEdgeCount() const;
	virtual unsigned int getInEdgeCount() const;

	bool getIsActive() const;
	void setIsActive(bool isActive);

	QtGraphNode* getParent() const;
	void setParent(std::weak_ptr<QtGraphNode> parentNode);

	void addComponent(const std::shared_ptr<QtGraphNodeComponent>& component);

	std::list<std::shared_ptr<QtGraphNode>> getSubNodes() const;
	void addSubNode(const std::shared_ptr<QtGraphNode>& node);

	/**
	 * @brief Returns the count of all connected edges and active nodes (including sub nodes)
	 */
	unsigned int getEdgeAndActiveCountRecursive() const;

	virtual void hideContent();

	virtual void onClick();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	void notifyEdgesAfterMove();
	void onSizeChanged();

	virtual void rebuildLayout();

	std::list<std::shared_ptr<GraphEdge>> m_outEdges;
	std::list<std::weak_ptr<GraphEdge>> m_inEdges;

	std::weak_ptr<QtGraphNode> m_parentNode;
	std::list<std::shared_ptr<QtGraphNode>> m_subNodes;

	QGraphicsSimpleTextItem* m_text;
	QtGraphicsRoundedRectItem* m_rect;
	QtGraphicsRoundedRectItem* m_undefinedRect;

	Vec2i m_baseSize;
	Vec2i m_currentSize;

	Vec4i m_padding;
	int m_spacing;

private:
	void setStyle();

	std::list<std::shared_ptr<QtGraphNodeComponent>> m_components;

	bool m_isActive;
	bool m_isHovering;
};

#endif // QT_GRAPH_NODE_H
