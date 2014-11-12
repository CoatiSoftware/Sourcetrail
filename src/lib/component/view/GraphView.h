#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include <list>
#include <memory>

#include "utility/types.h"

#include "component/view/View.h"

struct DummyEdge;
struct DummyNode;
class Graph;

class GraphView : public View
{
public:
	GraphView(ViewLayout* viewLayout);
	virtual ~GraphView();

	virtual std::string getName() const;

	virtual void rebuildGraph(
		std::shared_ptr<Graph> graph,
		std::vector<Id> activeTokenIds,
		const std::vector<DummyNode>& nodes,
		const std::vector<DummyEdge>& edges) = 0;

	virtual void clear() = 0;
};

#endif // GRAPH_VIEW_H