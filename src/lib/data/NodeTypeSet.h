#ifndef NODE_TYPE_SET_H
#define NODE_TYPE_SET_H

#include <unordered_set>
#include <unordered_set>

#include "data/NodeType.h"
#include "utility/types.h"

class NodeTypeSet
{
public:
	static NodeTypeSet all();
	static NodeTypeSet none();

	NodeTypeSet();
	NodeTypeSet(const NodeType& type);

	bool operator==(const NodeTypeSet& other) const;
	bool operator!=(const NodeTypeSet& other) const;

	void invert();
	NodeTypeSet getInverse() const;

	void add(const NodeTypeSet& typeSet);
	std::set<NodeType> getNodeTypes() const;

	void remove(const NodeTypeSet& typeSet);
	NodeTypeSet getWithRemoved(const NodeTypeSet& typeSet) const;

	bool isEmpty() const;
	bool contains(const NodeType& type) const;
	bool intersectsWith(const NodeTypeSet& typeSet) const;
	std::vector<Id> getNodeTypeIds() const;

private:
	std::set<NodeType> m_nodeTypes;
};

#endif // NODE_TYPE_SET_H
