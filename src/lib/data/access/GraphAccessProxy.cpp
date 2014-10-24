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

std::vector<SearchIndex::SearchMatch> GraphAccessProxy::getAutocompletionMatches(const std::string& query) const
{
	if (hasSubject())
	{
		return m_subject->getAutocompletionMatches(query);
	}

	return std::vector<SearchIndex::SearchMatch>();
}

std::shared_ptr<Graph> GraphAccessProxy::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForActiveTokenIds(tokenIds);
	}

	return std::make_shared<Graph>();
}

std::vector<Id> GraphAccessProxy::getActiveTokenIdsForId(Id tokenId, Id& delcarationId) const
{
	if (hasSubject())
	{
		return m_subject->getActiveTokenIdsForId(tokenId, delcarationId);
	}

	return std::vector<Id>();
}

std::vector<Id> GraphAccessProxy::getLocationIdsForTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getLocationIdsForTokenIds(tokenIds);
	}

	return std::vector<Id>();
}

std::vector<Id> GraphAccessProxy::getTokenIdsForQuery(std::string query) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdsForQuery(query);
	}

	return std::vector<Id>();
}
