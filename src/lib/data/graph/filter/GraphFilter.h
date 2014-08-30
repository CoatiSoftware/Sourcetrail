#ifndef GRAPH_FILTER_H
#define GRAPH_FILTER_H

class Edge;
class FilterableGraph;
class Node;

class GraphFilter
{
public:
	GraphFilter();
	virtual ~GraphFilter();

	void apply(const FilterableGraph* in, FilterableGraph* out);

protected:
	virtual void visitNode(Node* node);
	// virtual void visitEdge(Edge* edge);

	void addNode(Node* node);
	void addEdge(Edge* edge);

private:
	FilterableGraph* m_outGraph;
};

#endif // GRAPH_FILTER_H
