#include "data/access/StorageAccessProxy.h"

#include "data/graph/Graph.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"

#include "utility/logging/logging.h"
#include "utility/file/FileInfo.h"

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

Id StorageAccessProxy::getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const
{
	if (hasSubject())
	{
		return m_subject->getIdForNodeWithNameHierarchy(nameHierarchy);
	}

	return 0;
}

Id StorageAccessProxy::getIdForEdge(
	Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy
) const {
	if (hasSubject())
	{
		return m_subject->getIdForEdge(type, fromNameHierarchy, toNameHierarchy);
	}

	return 0;
}

std::vector<FileInfo> StorageAccessProxy::getInfoOnAllFiles() const
{
	std::vector<FileInfo> fileInfos;
	if (hasSubject())
	{
		fileInfos = m_subject->getInfoOnAllFiles();
	}
	return fileInfos;
}

Node::NodeType StorageAccessProxy::getNodeTypeForNodeWithId(Id id) const
{
	if(hasSubject())
	{
		return m_subject->getNodeTypeForNodeWithId(id);
	}
	return Node::NODE_UNDEFINED;
}

NameHierarchy StorageAccessProxy::getNameHierarchyForNodeWithId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNameHierarchyForNodeWithId(id);
	}

	return NameHierarchy();
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

std::vector<Id> StorageAccessProxy::getActiveTokenIdsForTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getActiveTokenIdsForTokenIds(tokenIds);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getActiveTokenIdsForId(Id tokenId, Id* delcarationId) const
{
	if (hasSubject())
	{
		return m_subject->getActiveTokenIdsForId(tokenId, delcarationId);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getNodeIdsForLocationIds(locationIds);
	}

	return std::vector<Id>();
}

std::vector<Id> StorageAccessProxy::getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdsForMatches(matches);
	}

	return std::vector<Id>();
}

Id StorageAccessProxy::getTokenIdForFileNode(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdForFileNode(filePath);
	}

	return 0;
}

std::vector<Id> StorageAccessProxy::getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const
{
	if (hasSubject())
	{
		return m_subject->getTokenIdsForAggregationEdge(sourceId, targetId);
	}

	return std::vector<Id>();
}

std::shared_ptr<TokenLocationCollection> StorageAccessProxy::getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForTokenIds(tokenIds);
	}

	return std::make_shared<TokenLocationCollection>();
}

std::shared_ptr<TokenLocationCollection> StorageAccessProxy::getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLocationIds(locationIds);
	}

	return std::make_shared<TokenLocationCollection>();
}

std::shared_ptr<TokenLocationFile> StorageAccessProxy::getTokenLocationsForFile(const std::string& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForFile(filePath);
	}

	return std::make_shared<TokenLocationFile>("");
}

std::shared_ptr<TokenLocationFile> StorageAccessProxy::getTokenLocationsForLinesInFile(
	const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
	}

	return std::make_shared<TokenLocationFile>("");
}

TokenLocationCollection StorageAccessProxy::getErrorTokenLocations(std::vector<std::string>* errorMessages) const
{
	if (hasSubject())
	{
		return m_subject->getErrorTokenLocations(errorMessages);
	}

	return TokenLocationCollection();
}

std::shared_ptr<TokenLocationFile> StorageAccessProxy::getTokenLocationOfParentScope(const TokenLocation* child) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationOfParentScope(child);
	}

	return std::make_shared<TokenLocationFile>("");
}

std::shared_ptr<TextAccess> StorageAccessProxy::getFileContent(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getFileContent(filePath);
	}

	return nullptr;
}
