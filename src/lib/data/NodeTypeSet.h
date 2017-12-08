#ifndef NODE_TYPE_SET_H
#define NODE_TYPE_SET_H

#include <functional>
#include <vector>

#include "utility/types.h"

class NodeType;

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
	std::vector<NodeType> getNodeTypes() const;

	void remove(const NodeTypeSet& typeSet);
	NodeTypeSet getWithRemoved(const NodeTypeSet& typeSet) const;

	void removeIf(const std::function<bool(const NodeType&)> condition);
	NodeTypeSet getWithRemovedIf(const std::function<bool(const NodeType&)> condition) const;

	bool isEmpty() const;
	bool contains(const NodeType& type) const;
	bool intersectsWith(const NodeTypeSet& typeSet) const;
	std::vector<Id> getNodeTypeIds() const;

private:
	typedef unsigned long int MaskType;

	static MaskType nodeTypeToMask(const NodeType& nodeType);
	static const std::vector<NodeType> s_allNodeTypes;

	MaskType m_nodeTypeMask;
};

#endif // NODE_TYPE_SET_H
