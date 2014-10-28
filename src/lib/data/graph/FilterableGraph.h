#ifndef FILTERABLE_GRAPH_H
#define FILTERABLE_GRAPH_H

#include <functional>
#include <ostream>

#include "utility/types.h"

class Edge;
class Node;
class Token;

class FilterableGraph
{
public:
	FilterableGraph();
	virtual ~FilterableGraph();

	virtual void copy(const FilterableGraph* other) = 0;
	virtual void clear() = 0;

	virtual void add(const FilterableGraph* other) = 0;

	virtual void forEachNode(std::function<void(Node*)> func) const = 0;
	virtual void forEachEdge(std::function<void(Edge*)> func) const = 0;
	virtual void forEachToken(std::function<void(Token*)> func) const = 0;

	virtual void addNode(Node* node) = 0;
	virtual void addEdge(Edge* edge) = 0;

	virtual size_t getNodeCount() const = 0;
	virtual size_t getEdgeCount() const = 0;

	virtual Node* getNodeById(Id id) const = 0;
	virtual Edge* getEdgeById(Id id) const = 0;

	size_t size() const;

	Token* getTokenById(Id id) const;

	void print(std::ostream& ostream) const;
	void printBasic(std::ostream& ostream) const;
};

#endif // FILTERABLE_GRAPH_H
