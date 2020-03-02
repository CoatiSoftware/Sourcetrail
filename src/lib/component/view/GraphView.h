#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include <vector>

#include "Vector2.h"
#include "types.h"

#include "GroupType.h"
#include "ScreenSearchInterfaces.h"
#include "View.h"

struct DummyEdge;
struct DummyNode;
class Graph;

class GraphView
	: public View
	, public ScreenSearchResponder
{
public:
	static const char* VIEW_NAME;

	struct GraphParams
	{
		bool animatedTransition = true;
		bool centerActiveNode = false;
		bool scrollToTop = false;
		bool isIndexedList = false;
		bool bezierEdges = false;
		bool disableInteraction = false;
		Id tokenIdToFocus = 0;
	};

	GraphView(ViewLayout* viewLayout);
	virtual ~GraphView();

	virtual std::string getName() const;

	virtual void rebuildGraph(
		std::shared_ptr<Graph> graph,
		const std::vector<std::shared_ptr<DummyNode>>& nodes,
		const std::vector<std::shared_ptr<DummyEdge>>& edges,
		const GraphParams params) = 0;
	virtual void clear() = 0;

	virtual void coFocusTokenIds(const std::vector<Id>& focusedTokenIds) = 0;
	virtual void deCoFocusTokenIds(const std::vector<Id>& defocusedTokenIds) = 0;

	virtual void resizeView() = 0;

	virtual Vec2i getViewSize() const = 0;
	virtual GroupType getGrouping() const = 0;

	virtual void scrollToValues(int xValue, int yValue) = 0;

	virtual void activateEdge(Id edgeId) = 0;

	virtual void setNavigationFocus(bool focus) = 0;
	virtual bool hasNavigationFocus() const = 0;
};

#endif	  // GRAPH_VIEW_H
