#include "data/access/StorageAccessProxy.h"

#include "utility/logging/logging.h"

#include "data/graph/Graph.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"

StorageAccessProxy::StorageAccessProxy()
	: m_subject(nullptr)
{
}

StorageAccessProxy::~StorageAccessProxy()
{
}

bool StorageAccessProxy::hasSubject() const
{
	if (m_subject)
	{
		return true;
	}

	LOG_ERROR("StorageAccessProxy has no subject.");
	return false;
}

void StorageAccessProxy::setSubject(StorageAccess* subject)
{
	m_subject = subject;
}

Id StorageAccessProxy::getIdForNodeWithName(const std::string& name) const
{
	if (hasSubject())
	{
		return m_subject->getIdForNodeWithName(name);
	}

	return 0;
}

Node::NodeType StorageAccessProxy::getNodeTypeForNodeWithId(Id id) const
{
	if(hasSubject())
	{
		return m_subject->getNodeTypeForNodeWithId(id);
	}
	return Node::NODE_UNDEFINED;
}

std::string StorageAccessProxy::getNameForNodeWithId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNameForNodeWithId(id);
	}

	return "";
}

std::vector<SearchMatch> StorageAccessProxy::getAutocompletionMatches(
	const std::string& query,
	const std::string& word
) const {
	if (hasSubject())
	{
		return m_subject->getAutocompletionMatches(query, word);
	}

	return std::vector<SearchMatch>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForActiveTokenIds(tokenIds);
	}

	return std::make_shared<Graph>();
}

std::vector<Id> StorageAccessProxy::getActiveTokenIdsForId(Id tokenId, Id* delcarationId) const
{
	if (hasSubject())
	{
		return m_subject->getActiveTokenIdsForId(tokenId, delcarationId);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getActiveTokenIdsForLocationId(Id locationId) const
{
	if (hasSubject())
	{
		return m_subject->getActiveTokenIdsForLocationId(locationId);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getTokenIdsForQuery(std::string query) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdsForQuery(query);
	}

	return std::vector<Id>();
}

TokenLocationCollection StorageAccessProxy::getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForTokenIds(tokenIds);
	}

	return TokenLocationCollection();
}

TokenLocationFile StorageAccessProxy::getTokenLocationsForFile(const std::string& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForFile(filePath);
	}

	return TokenLocationFile("");
}

TokenLocationFile StorageAccessProxy::getTokenLocationsForLinesInFile(
	const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
	}

	return TokenLocationFile("");
}

TokenLocationCollection StorageAccessProxy::getErrorTokenLocations(std::vector<std::string>* errorMessages) const
{
	if (hasSubject())
	{
		return m_subject->getErrorTokenLocations(errorMessages);
	}

	return TokenLocationCollection();
}
