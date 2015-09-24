#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <QGraphicsItem>

#include "utility/math/Vector4.h"

#include "component/view/GraphViewStyle.h"

class QFont;
class QtGraphEdge;
class QtRoundedRectItem;
class QtGraphNodeComponent;

class QtGraphNode
	: public QObject
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
	virtual ~QtGraphNode();

	QtGraphNode* getParent() const;
	QtGraphNode* getLastParent() const;
	void setParent(std::weak_ptr<QtGraphNode> parentNode);

	std::list<std::shared_ptr<QtGraphNode>> getSubNodes() const;

	Vec2i getPosition() const;
	bool setPosition(const Vec2i& position);

	Vec2i getSize() const;
	void setSize(const Vec2i& size);

	QSize size() const;
	void setSize(const QSize& size);

	Vec4i getBoundingRect() const;
	Vec4i getParentBoundingRect() const;

	void addOutEdge(const std::shared_ptr<QtGraphEdge>& edge);
	void addInEdge(const std::weak_ptr<QtGraphEdge>& edge);

	size_t getOutEdgeCount() const;
	size_t getInEdgeCount() const;

	bool getIsActive() const;
	void setIsActive(bool isActive);

	std::string getName() const;
	void setName(const std::string& name);

	void addComponent(const std::shared_ptr<QtGraphNodeComponent>& component);

	void setShadowEnabledRecursive(bool enabled);

	void hoverEnter();

	void focusIn();
	void focusOut();

	virtual bool isDataNode() const;
	virtual bool isAccessNode() const;
	virtual bool isExpandToggleNode() const;
	virtual bool isBundleNode() const;

	virtual Id getTokenId() const;

	virtual void addSubNode(const std::shared_ptr<QtGraphNode>& node);

	virtual void onClick();
	virtual void moved(const Vec2i& oldPosition);

	virtual void updateStyle() = 0;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	void notifyEdgesAfterMove();

	void setStyle(const GraphViewStyle::NodeStyle& style);

	std::list<std::shared_ptr<QtGraphEdge>> m_outEdges;
	std::list<std::weak_ptr<QtGraphEdge>> m_inEdges;

	std::weak_ptr<QtGraphNode> m_parentNode;
	std::list<std::shared_ptr<QtGraphNode>> m_subNodes;

	QGraphicsSimpleTextItem* m_text;
	QtRoundedRectItem* m_rect;
	QtRoundedRectItem* m_undefinedRect;
	QGraphicsPixmapItem* m_icon;

	Vec2i m_size;

	bool m_isActive;
	bool m_isHovering;

private:
	std::list<std::shared_ptr<QtGraphNodeComponent>> m_components;
};

#endif // QT_GRAPH_NODE_H
