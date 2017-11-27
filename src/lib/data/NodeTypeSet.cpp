#include "data/NodeTypeSet.h"

#include "utility/utility.h"

NodeTypeSet NodeTypeSet::all()
{
	NodeTypeSet ret;

	ret.add(NodeType(NodeType::NODE_SYMBOL));
	ret.add(NodeType(NodeType::NODE_TYPE));
	ret.add(NodeType(NodeType::NODE_BUILTIN_TYPE));
	ret.add(NodeType(NodeType::NODE_NAMESPACE));
	ret.add(NodeType(NodeType::NODE_PACKAGE));
	ret.add(NodeType(NodeType::NODE_STRUCT));
	ret.add(NodeType(NodeType::NODE_CLASS));
	ret.add(NodeType(NodeType::NODE_INTERFACE));
	ret.add(NodeType(NodeType::NODE_GLOBAL_VARIABLE));
	ret.add(NodeType(NodeType::NODE_FIELD));
	ret.add(NodeType(NodeType::NODE_FUNCTION));
	ret.add(NodeType(NodeType::NODE_METHOD));
	ret.add(NodeType(NodeType::NODE_ENUM));
	ret.add(NodeType(NodeType::NODE_ENUM_CONSTANT));
	ret.add(NodeType(NodeType::NODE_TYPEDEF));
	ret.add(NodeType(NodeType::NODE_TEMPLATE_PARAMETER_TYPE));
	ret.add(NodeType(NodeType::NODE_TYPE_PARAMETER));
	ret.add(NodeType(NodeType::NODE_FILE));
	ret.add(NodeType(NodeType::NODE_MACRO));
	ret.add(NodeType(NodeType::NODE_UNION));

	return ret;
}

NodeTypeSet NodeTypeSet::none()
{
	return NodeTypeSet();
}

NodeTypeSet::NodeTypeSet()
{
	std::string i;
}

NodeTypeSet::NodeTypeSet(const NodeType& type)
{
	m_nodeTypes.insert(type);
}

bool NodeTypeSet::operator==(const NodeTypeSet& other) const
{
	return utility::isPermutation(getNodeTypeIds(), other.getNodeTypeIds()); // TODO: reimplement using mask
}

bool NodeTypeSet::operator!=(const NodeTypeSet& other) const
{
	return !operator==(other);
}

void NodeTypeSet::invert()
{
	NodeTypeSet inverse = NodeTypeSet::all().getWithRemoved(*this);
	m_nodeTypes = std::move(inverse.m_nodeTypes);
}

NodeTypeSet NodeTypeSet::getInverse() const
{
	NodeTypeSet ret(*this);
	ret.invert();
	return ret;
}

void NodeTypeSet::add(const NodeTypeSet& typeSet)
{
	utility::append(m_nodeTypes, typeSet.m_nodeTypes);
}

std::set<NodeType> NodeTypeSet::getNodeTypes() const
{
	return m_nodeTypes;
}

void NodeTypeSet::remove(const NodeTypeSet& typeSet)
{
	for (const NodeType& type : typeSet.getNodeTypes())
	{
		std::set<NodeType>::const_iterator it = m_nodeTypes.find(type);
		if (it != m_nodeTypes.end())
		{
			m_nodeTypes.erase(it);
		}
	}
}

NodeTypeSet NodeTypeSet::getWithRemoved(const NodeTypeSet& typeSet) const
{
	NodeTypeSet ret(*this);
	ret.remove(typeSet);
	return ret;
}

bool NodeTypeSet::isEmpty() const
{
	return m_nodeTypes.empty();
}

bool NodeTypeSet::contains(const NodeType& type) const
{
	return m_nodeTypes.find(type) != m_nodeTypes.end();
}

bool NodeTypeSet::intersectsWith(const NodeTypeSet& typeSet) const
{
	for (const NodeType& type : typeSet.getNodeTypes())
	{
		std::set<NodeType>::const_iterator it = m_nodeTypes.find(type);
		if (it != m_nodeTypes.end())
		{
			return true;
		}
	}
	return false;
}

std::vector<Id> NodeTypeSet::getNodeTypeIds() const
{
	std::vector<Id> ids;

	for (const NodeType type : m_nodeTypes)
	{
		ids.push_back(type.getId());
	}

	return ids;
}
