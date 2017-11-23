#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <functional>

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
	static QFont getFontForStyleType(NodeType::StyleType type);

	QtGraphNode();
	virtual ~QtGraphNode();

	QtGraphNode* getParent() const;
	QtGraphNode* getLastParent() const;
	void setParent(QtGraphNode* parentNode);

	std::list<QtGraphNode*> getSubNodes() const;

	Vec2i getPosition() const;
	virtual bool setPosition(const Vec2i& position);

	Vec2i getSize() const;
	void setSize(const Vec2i& size);

	QSize size() const;
	void setSize(const QSize& size);

	Vec4i getBoundingRect() const;
	Vec4i getParentBoundingRect() const;

	void addOutEdge(QtGraphEdge* edge);
	void addInEdge(QtGraphEdge* edge);

	size_t getOutEdgeCount() const;
	size_t getInEdgeCount() const;

	bool getIsActive() const;
	void setIsActive(bool isActive);
	void setMultipleActive(bool multipleActive);

	std::string getName() const;
	void setName(const std::string& name);

	void addComponent(const std::shared_ptr<QtGraphNodeComponent>& component);

	void hoverEnter();

	void focusIn();
	void focusOut();

	void showNodeRecursive();

	void matchNameRecursive(const std::string& query, std::vector<QtGraphNode*>* matchedNodes);
	void removeNameMatch();
	void setActiveMatch(bool active);

	virtual bool isDataNode() const;
	virtual bool isAccessNode() const;
	virtual bool isExpandToggleNode() const;
	virtual bool isBundleNode() const;
	virtual bool isQualifierNode() const;
	virtual bool isTextNode() const;

	virtual Id getTokenId() const;

	virtual void addSubNode(QtGraphNode* node);

	virtual void onClick();
	virtual void moved(const Vec2i& oldPosition);

	virtual void updateStyle() = 0;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	void forEachEdge(std::function<void(QtGraphEdge*)> func);

	void notifyEdgesAfterMove();

	virtual void matchName(const std::string& query, std::vector<QtGraphNode*>* matchedNodes);

	void setStyle(const GraphViewStyle::NodeStyle& style);

	std::list<QtGraphEdge*> m_outEdges;
	std::list<QtGraphEdge*> m_inEdges;

	QtGraphNode* m_parentNode = nullptr;
	std::list<QtGraphNode*> m_subNodes;

	QGraphicsSimpleTextItem* m_text = nullptr;
	QtRoundedRectItem* m_rect = nullptr;
	QtRoundedRectItem* m_undefinedRect = nullptr;
	QGraphicsPixmapItem* m_icon = nullptr;

	Vec2i m_size;

	bool m_isActive = false;
	bool m_multipleActive = false;
	bool m_isHovering = false;

private:
	std::list<std::shared_ptr<QtGraphNodeComponent>> m_components;

	// Name match
	QGraphicsSimpleTextItem* m_matchText = nullptr;
	QtRoundedRectItem* m_matchRect = nullptr;
	size_t m_matchPos = 0;
	size_t m_matchLength = 0;
	bool m_isActiveMatch = false;
};

#endif // QT_GRAPH_NODE_H
