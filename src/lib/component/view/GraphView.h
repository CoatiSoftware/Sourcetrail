#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include <vector>

#include "utility/math/Vector2.h"
#include "utility/types.h"

#include "component/controller/helper/ScreenSearchInterfaces.h"
#include "component/view/View.h"
#include "data/GroupType.h"

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
		bool animatedTransition;
		bool centerActiveNode;
		bool scrollToTop;
		bool isIndexedList;
		bool bezierEdges;
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

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds) = 0;
	virtual void defocusTokenIds(const std::vector<Id>& defocusedTokenIds) = 0;

	virtual void resizeView() = 0;

	virtual Vec2i getViewSize() const = 0;
	virtual GroupType getGrouping() const = 0;

	virtual void scrollToValues(int xValue, int yValue) = 0;

	virtual void activateEdge(Id edgeId, bool centerOrigin) = 0;
};

#endif // GRAPH_VIEW_H
