#ifndef EDGE_COMPONENT_H
#define EDGE_COMPONENT_H

#include "data/graph/Edge.h"

class EdgeComponent
{
public:
	EdgeComponent();
	virtual ~EdgeComponent();

	virtual std::shared_ptr<EdgeComponent> copy() const = 0;

	void setEdge(Edge* edge);

protected:
	Edge* getEdge() const;

private:
	Edge* m_edge;
};

#endif // EDGE_COMPONENT_H
