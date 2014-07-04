#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include <list>
#include <memory>

#include "component/view/View.h"

class GraphEdge;
class GraphNode;

class GraphView : public View
{
public:
	GraphView(ViewLayout* viewLayout);
	virtual ~GraphView();

	virtual std::string getName() const;

	virtual std::weak_ptr<GraphNode> addNode(const Vec2i& position, const std::string& name) = 0;
	virtual void addEdge(const std::weak_ptr<GraphNode>& owner, const std::weak_ptr<GraphNode>& target) = 0;

protected:
	std::list<std::shared_ptr<GraphNode> > m_nodes;
	std::list<std::weak_ptr<GraphEdge> > m_edges;
};

#endif // GRAPH_VIEW_H