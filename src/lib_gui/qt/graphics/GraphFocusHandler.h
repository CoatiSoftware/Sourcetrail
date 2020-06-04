#ifndef GRAPH_FOCUS_HANDLER_H
#define GRAPH_FOCUS_HANDLER_H

#include <list>
#include <vector>

#include "types.h"

class QtGraphEdge;
class QtGraphNode;
class QPointF;

class GraphFocusClient
{
public:
	virtual void focusView(bool focusIn) = 0;

	virtual const std::list<QtGraphNode*>& getGraphNodes() const = 0;
	virtual const std::list<QtGraphEdge*>& getGraphEdges() const = 0;

	virtual QtGraphNode* getActiveNode() const = 0;

	virtual void ensureNodeVisible(QtGraphNode* node) = 0;
};

class GraphFocusHandler
{
public:
	enum class Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	GraphFocusHandler(GraphFocusClient* client);

	void clear();

	void focus(bool focusIn);

	void focusInitialNode();
	void focusTokenId(
		const std::list<QtGraphNode*>& nodes, const std::list<QtGraphEdge*>& edges, Id tokenId);
	void refocusNode(const std::list<QtGraphNode*>& newNodes, Id oldActiveTokenId, Id newActiveTokenId);

	void focusNext(Direction direction, bool navigateEdges);

	void focusNode(QtGraphNode* node);
	void defocusNode(QtGraphNode* node);

	void focusEdge(QtGraphEdge* edge);
	void defocusEdge(QtGraphEdge* edge);

	void defocusGraph();

	void activateFocus(bool openInTab);
	void expandFocus();

private:
	QtGraphNode* findNextNode(QtGraphNode* node, Direction direction);
	QtGraphNode* findNextNode(QtGraphEdge* edge, Direction direction);
	QtGraphEdge* findNextEdge(QPointF pos, Direction direction, QtGraphEdge* previousEdge = nullptr);

	QtGraphNode* findChildNodeRecursive(const std::list<QtGraphNode*>& nodes, bool first);

	QtGraphNode* findSibling(const QtGraphNode* node, Direction direction);
	std::vector<std::vector<QtGraphNode*>> getSiblingsHierarchyRecursive(const QtGraphNode* node);
	void addSiblingsRecursive(const std::list<QtGraphNode*>& nodes, std::vector<QtGraphNode*>& siblings);

	GraphFocusClient* const m_client;

	QtGraphNode* m_focusNode = nullptr;
	QtGraphEdge* m_focusEdge = nullptr;
	Id m_lastFocusId = 0;
};

#endif	  // GRAPH_FOCUS_HANDLER_H
