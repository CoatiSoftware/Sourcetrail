#include "data/access/GraphAccessProxy.h"

#include "utility/logging/logging.h"

GraphAccessProxy::GraphAccessProxy()
	: m_subject(nullptr)
{
}

GraphAccessProxy::~GraphAccessProxy()
{
}

bool GraphAccessProxy::hasSubject() const
{
	if (m_subject)
	{
		return true;
	}

	LOG_ERROR("GraphAccessProxy has no subject.");
	return false;
}

void GraphAccessProxy::setSubject(GraphAccess* subject)
{
	m_subject = subject;
}

Id GraphAccessProxy::getIdForNodeWithName(const std::string& name) const
{
	if (hasSubject())
	{
		return m_subject->getIdForNodeWithName(name);
	}

	return 0;
}

std::string GraphAccessProxy::getNameForNodeWithId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNameForNodeWithId(id);
	}

	return "";
}

std::vector<std::string> GraphAccessProxy::getNamesForNodesWithNamePrefix(const std::string& prefix) const
{
	if (hasSubject())
	{
		return m_subject->getNamesForNodesWithNamePrefix(prefix);
	}

	return std::vector<std::string>();
}

std::vector<Id> GraphAccessProxy::getIdsOfNeighbours(const Id id) const
{
	if (hasSubject())
	{
		return m_subject->getIdsOfNeighbours(id);
	}

	return std::vector<Id>();
}

std::vector<std::pair<Id, Id>> GraphAccessProxy::getConnectedEdges(const Id id) const
{
	if (hasSubject())
	{
		return m_subject->getConnectedEdges(id);
	}

	return std::vector<std::pair<Id, Id>>();
}
