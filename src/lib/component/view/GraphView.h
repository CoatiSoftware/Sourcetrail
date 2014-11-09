#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include <list>
#include <memory>

#include "utility/types.h"

#include "component/view/View.h"

struct DummyEdge;
struct DummyNode;
class Graph;
class GraphEdge;
class GraphNode;

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

protected:
	std::shared_ptr<Graph> m_graph;
	std::vector<Id> m_activeTokenIds;

	std::list<std::shared_ptr<GraphNode> > m_nodes;
	std::list<std::weak_ptr<GraphEdge> > m_edges;
};

#endif // GRAPH_VIEW_H