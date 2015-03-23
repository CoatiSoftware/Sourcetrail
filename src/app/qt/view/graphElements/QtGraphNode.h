#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <QFontMetrics>
#include <QGraphicsItem>

#include "component/view/graphElements/GraphNode.h"
#include "component/view/GraphViewStyle.h"

class QtGraphEdge;
class QtRoundedRectItem;
class QtGraphNodeComponent;

class QtGraphNode
	: public QObject
	, public GraphNode
	, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(QSize size READ size WRITE setSize)

public slots:
	void blendIn();
	void blendOut();

	void showNode();
	void hideNode();

public:
	static QFont getFontForNodeType(Node::NodeType type);

	QtGraphNode();
	QtGraphNode(const Node* data);
	virtual ~QtGraphNode();

	virtual std::string getName() const;
	void setName(const std::string& name);

	virtual bool isAccessNode() const;

	virtual Vec2i getPosition() const;
	virtual bool setPosition(const Vec2i& position);
	virtual void moveTo(const Vec2i& position);

	virtual Vec2i getSize() const;
	virtual void setSize(const Vec2i& size);

	virtual Vec4i getBoundingRect() const;
	virtual Vec4i getParentBoundingRect() const;

	virtual void addOutEdge(const std::shared_ptr<GraphEdge>& edge);
	virtual void addInEdge(const std::weak_ptr<GraphEdge>& edge);

	virtual size_t getOutEdgeCount() const;
	virtual size_t getInEdgeCount() const;

	QSize size() const;
	void setSize(const QSize& size);

	bool getIsActive() const;
	void setIsActive(bool isActive);

	QtGraphNode* getParent() const;
	void setParent(std::weak_ptr<QtGraphNode> parentNode);

	void addComponent(const std::shared_ptr<QtGraphNodeComponent>& component);

	std::list<std::shared_ptr<QtGraphNode>> getSubNodes() const;
	virtual void addSubNode(const std::shared_ptr<QtGraphNode>& node);

	void setShadowEnabledRecursive(bool enabled);

	virtual void onClick();
	void hoverEnter();

	virtual void updateStyle();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

	void notifyEdgesAfterMove();

	void setStyle(const GraphViewStyle::NodeStyle& style);

	std::list<std::shared_ptr<GraphEdge>> m_outEdges;
	std::list<std::weak_ptr<GraphEdge>> m_inEdges;

	std::weak_ptr<QtGraphNode> m_parentNode;
	std::list<std::shared_ptr<QtGraphNode>> m_subNodes;

	QGraphicsSimpleTextItem* m_text;
	QtRoundedRectItem* m_rect;
	QtRoundedRectItem* m_undefinedRect;

	Vec2i m_size;

private:
	std::list<std::shared_ptr<QtGraphNodeComponent>> m_components;

	bool m_isActive;
	bool m_isHovering;
};

#endif // QT_GRAPH_NODE_H
