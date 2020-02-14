#include "NodeTypeSet.h"

#include "NodeType.h"
#include "utility.h"

NodeTypeSet NodeTypeSet::all()
{
	NodeTypeSet ret;
	ret.m_nodeTypeMask = ~0;
	return ret;
}

NodeTypeSet NodeTypeSet::none()
{
	return NodeTypeSet();
}

NodeTypeSet::NodeTypeSet()
{
	m_nodeTypeMask = 0;
}

NodeTypeSet::NodeTypeSet(const NodeType& type): m_nodeTypeMask(nodeTypeToMask(type)) {}

bool NodeTypeSet::operator==(const NodeTypeSet& other) const
{
	return m_nodeTypeMask == other.m_nodeTypeMask;
}

bool NodeTypeSet::operator!=(const NodeTypeSet& other) const
{
	return !operator==(other);
}

std::vector<NodeType> NodeTypeSet::getNodeTypes() const
{
	std::vector<NodeType> nodeTypes;

	for (const NodeType& type: s_allNodeTypes)
	{
		if (m_nodeTypeMask & nodeTypeToMask(type))
		{
			nodeTypes.push_back(type);
		}
	}

	return nodeTypes;
}

void NodeTypeSet::invert()
{
	m_nodeTypeMask = ~m_nodeTypeMask;
}

NodeTypeSet NodeTypeSet::getInverse() const
{
	NodeTypeSet ret(*this);
	ret.invert();
	return ret;
}

void NodeTypeSet::add(const NodeTypeSet& typeSet)
{
	m_nodeTypeMask |= typeSet.m_nodeTypeMask;
}

NodeTypeSet NodeTypeSet::getWithAdded(const NodeTypeSet& typeSet) const
{
	return NodeTypeSet(m_nodeTypeMask | typeSet.m_nodeTypeMask);
}

void NodeTypeSet::remove(const NodeTypeSet& typeSet)
{
	m_nodeTypeMask &= ~typeSet.m_nodeTypeMask;
}

NodeTypeSet NodeTypeSet::getWithRemoved(const NodeTypeSet& typeSet) const
{
	return NodeTypeSet(m_nodeTypeMask & ~typeSet.m_nodeTypeMask);
}

void NodeTypeSet::keepMatching(const std::function<bool(const NodeType&)>& matcher)
{
	for (const NodeType& type: s_allNodeTypes)
	{
		if (m_nodeTypeMask & nodeTypeToMask(type) && !matcher(type))
		{
			remove(type);
		}
	}
}

NodeTypeSet NodeTypeSet::getWithMatchingKept(const std::function<bool(const NodeType&)>& matcher) const
{
	NodeTypeSet ret(*this);
	ret.keepMatching(matcher);
	return ret;
}

void NodeTypeSet::removeMatching(const std::function<bool(const NodeType&)>& matcher)
{
	for (const NodeType& type: s_allNodeTypes)
	{
		if (m_nodeTypeMask & nodeTypeToMask(type) && matcher(type))
		{
			remove(type);
		}
	}
}

NodeTypeSet NodeTypeSet::getWithMatchingRemoved(const std::function<bool(const NodeType&)>& matcher) const
{
	NodeTypeSet ret(*this);
	ret.removeMatching(matcher);
	return ret;
}

bool NodeTypeSet::isEmpty() const
{
	return m_nodeTypeMask == 0;
}

bool NodeTypeSet::contains(const NodeType& type) const
{
	return m_nodeTypeMask & nodeTypeToMask(type);
}

bool NodeTypeSet::containsMatching(const std::function<bool(const NodeType&)>& matcher) const
{
	for (const NodeType& type: s_allNodeTypes)
	{
		if (m_nodeTypeMask & nodeTypeToMask(type) && matcher(type))
		{
			return true;
		}
	}
	return false;
}

bool NodeTypeSet::intersectsWith(const NodeTypeSet& typeSet) const
{
	return m_nodeTypeMask & typeSet.m_nodeTypeMask;
}

std::vector<Id> NodeTypeSet::getNodeTypeIds() const
{
	std::vector<Id> ids;

	for (const NodeType& type: s_allNodeTypes)
	{
		if (m_nodeTypeMask & nodeTypeToMask(type))
		{
			ids.push_back(type.getId());
		}
	}

	return ids;
}

NodeTypeSet::NodeTypeSet(NodeTypeSet::MaskType typeMask): m_nodeTypeMask(typeMask) {}

NodeTypeSet::MaskType NodeTypeSet::nodeTypeToMask(const NodeType& nodeType)
{
	// todo: convert to mask if ids are not power of two anymore
	return static_cast<MaskType>(nodeType.getId());
}

const std::vector<NodeType> NodeTypeSet::s_allNodeTypes = {
	NodeType(NODE_SYMBOL),		 NodeType(NODE_TYPE),
	NodeType(NODE_BUILTIN_TYPE),	 NodeType(NODE_MODULE),
	NodeType(NODE_NAMESPACE),		 NodeType(NODE_PACKAGE),
	NodeType(NODE_STRUCT),		 NodeType(NODE_CLASS),
	NodeType(NODE_INTERFACE),		 NodeType(NODE_GLOBAL_VARIABLE),
	NodeType(NODE_FIELD),			 NodeType(NODE_FUNCTION),
	NodeType(NODE_METHOD),		 NodeType(NODE_ENUM),
	NodeType(NODE_ENUM_CONSTANT),	 NodeType(NODE_TYPEDEF),
	NodeType(NODE_TYPE_PARAMETER), NodeType(NODE_FILE),
	NodeType(NODE_MACRO),			 NodeType(NODE_UNION)};
