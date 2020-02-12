#ifndef QT_GRAPH_NODE_H
#define QT_GRAPH_NODE_H

#include <functional>

#include <QGraphicsItem>

#include "Vector4.h"

#include "GraphViewStyle.h"

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
	QtGraphNode();
	virtual ~QtGraphNode();

	QtGraphNode* getParent() const;
	QtGraphNode* getLastParent(bool noGroups = false) const;
	QtGraphNode* getLastNonGroupParent() const;
	void setParent(QtGraphNode* parentNode);

	const std::list<QtGraphNode*>& getSubNodes() const;

	Vec2i getPosition() const;
	virtual bool setPosition(const Vec2i& position);

	const Vec2i& getSize() const;
	void setSize(const Vec2i& size);

	const Vec2i& getColumnSize() const;
	void setColumnSize(const Vec2i& size);

	QSize size() const;
	void setSize(QSize size);

	Vec4i getBoundingRect() const;

	void addOutEdge(QtGraphEdge* edge);
	void addInEdge(QtGraphEdge* edge);

	size_t getOutEdgeCount() const;
	size_t getInEdgeCount() const;

	bool getIsActive() const;
	void setIsActive(bool isActive);
	void setMultipleActive(bool multipleActive);
	bool hasActiveChild() const;

	std::wstring getName() const;
	void setName(const std::wstring& name);

	void addComponent(const std::shared_ptr<QtGraphNodeComponent>& component);

	void hoverEnter();

	void focusIn();
	void focusOut();

	void showNodeRecursive();

	void matchNameRecursive(const std::wstring& query, std::vector<QtGraphNode*>* matchedNodes);
	void removeNameMatch();
	void setActiveMatch(bool active);

	virtual bool isDataNode() const;
	virtual bool isAccessNode() const;
	virtual bool isExpandToggleNode() const;
	virtual bool isBundleNode() const;
	virtual bool isQualifierNode() const;
	virtual bool isTextNode() const;
	virtual bool isGroupNode() const;

	virtual Id getTokenId() const;

	virtual void addSubNode(QtGraphNode* node);

	virtual void onClick();
	virtual void onMiddleClick();

	void onHide();
	void onCollapseExpand();
	void onShowDefinition(bool inIDE);

	virtual void moved(const Vec2i& oldPosition);

	virtual void updateStyle() = 0;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	void forEachEdge(std::function<void(QtGraphEdge*)> func);

	void notifyEdgesAfterMove();

	virtual void matchName(const std::wstring& query, std::vector<QtGraphNode*>* matchedNodes);

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
	Vec2i m_columnSize;

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

#endif	  // QT_GRAPH_NODE_H
