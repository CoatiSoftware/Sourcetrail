#ifndef EDGE_H
#define EDGE_H

#include <memory>

#include "data/Element.h"

class Node;

class Edge: public Element
{
public:
	Edge(std::weak_ptr<Node> from, std::weak_ptr<Node> to);
	~Edge();

private:
	const std::weak_ptr<Node> m_from;
	const std::weak_ptr<Node> m_to;
};


#endif // EDGE_H
