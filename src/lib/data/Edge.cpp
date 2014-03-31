#include "data/Edge.h"

#include "data/Node.h"

Edge::Edge(std::weak_ptr<Node> from, std::weak_ptr<Node> to)
: m_from(from)
, m_to(to)
{
}

Edge::~Edge()
{
}
