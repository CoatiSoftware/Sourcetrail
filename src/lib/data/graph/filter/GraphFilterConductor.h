#ifndef GRAPH_FILTER_CONDUCTOR_H
#define GRAPH_FILTER_CONDUCTOR_H

class FilterableGraph;
class QueryCommand;
class QueryNode;
class QueryOperator;
class QueryToken;
class QueryTree;

class GraphFilterConductor
{
public:
	GraphFilterConductor();
	~GraphFilterConductor();

	void filter(const QueryTree* tree, const FilterableGraph* in, FilterableGraph* out) const;

private:
	void filterRecursively(const QueryNode* node, const FilterableGraph* in, FilterableGraph* out) const;
	void filterOperatorNode(const QueryOperator* node, const FilterableGraph* in, FilterableGraph* out) const;
	void filterCommandNode(const QueryCommand* node, const FilterableGraph* in, FilterableGraph* out) const;
	void filterTokenNode(const QueryToken* node, const FilterableGraph* in, FilterableGraph* out) const;
};

#endif // GRAPH_FILTER_CONDUCTOR_H
