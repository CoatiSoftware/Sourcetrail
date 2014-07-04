#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

class GraphEdge
{
public:
	GraphEdge();
	virtual ~GraphEdge();

	virtual void ownerMoved() = 0;
	virtual void targetMoved() = 0;
};

#endif // GRAPH_EDGE_H