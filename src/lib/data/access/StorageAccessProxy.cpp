#include "data/access/StorageAccessProxy.h"

#include "data/graph/Graph.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"

#include "utility/file/FileInfo.h"
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageShowErrors.h"

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

	setErrorFilter(m_errorFilter);
}

Id StorageAccessProxy::getNodeIdForFileNode(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getNodeIdForFileNode(filePath);
	}

	return 0;
}

Id StorageAccessProxy::getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const
{
	if (hasSubject())
	{
		return m_subject->getNodeIdForNameHierarchy(nameHierarchy);
	}

	return 0;
}

std::vector<Id> StorageAccessProxy::getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const
{
	if (hasSubject())
	{
		return m_subject->getNodeIdsForNameHierarchies(nameHierarchies);
	}
	return std::vector<Id>();
}

NameHierarchy StorageAccessProxy::getNameHierarchyForNodeId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNameHierarchyForNodeId(id);
	}

	return NameHierarchy(NAME_DELIMITER_UNKNOWN);
}

std::vector<NameHierarchy> StorageAccessProxy::getNameHierarchiesForNodeIds(const std::vector<Id> nodeIds) const
{
	if (hasSubject())
	{
		return m_subject->getNameHierarchiesForNodeIds(nodeIds);
	}
	return std::vector<NameHierarchy>();
}

Node::NodeType StorageAccessProxy::getNodeTypeForNodeWithId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getNodeTypeForNodeWithId(id);
	}
	return Node::NODE_NON_INDEXED;
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

StorageEdge StorageAccessProxy::getEdgeById(Id edgeId) const
{
	if (hasSubject())
	{
		return m_subject->getEdgeById(edgeId);
	}

	return StorageEdge();
}

std::shared_ptr<SourceLocationCollection> StorageAccessProxy::getFullTextSearchLocations(
		const std::string &searchTerm, bool caseSensitive) const
{
	if (hasSubject())
	{
		return m_subject->getFullTextSearchLocations(searchTerm, caseSensitive);
	}

	return std::make_shared<SourceLocationCollection>();
}

std::vector<SearchMatch> StorageAccessProxy::getAutocompletionMatches(const std::string& query) const
{
	if (hasSubject())
	{
		return m_subject->getAutocompletionMatches(query);
	}

	return std::vector<SearchMatch>();
}

std::vector<SearchMatch> StorageAccessProxy::getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getSearchMatchesForTokenIds(tokenIds);
	}

	return std::vector<SearchMatch>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForAll() const
{
	if (hasSubject())
	{
		return m_subject->getGraphForAll();
	}

	return std::make_shared<Graph>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForActiveTokenIds(
	const std::vector<Id>& tokenIds, const std::vector<Id>& expandedNodeIds, bool* isActiveNamespace) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForActiveTokenIds(tokenIds, expandedNodeIds, isActiveNamespace);
	}

	return std::make_shared<Graph>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForChildrenOfNodeId(Id nodeId) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForChildrenOfNodeId(nodeId);
	}

	return std::make_shared<Graph>();
}

std::shared_ptr<Graph> StorageAccessProxy::getGraphForTrail(Id originId, Id targetId, Edge::EdgeTypeMask trailType, size_t depth) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForTrail(originId, targetId, trailType, depth);
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

std::vector<Id> StorageAccessProxy::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getNodeIdsForLocationIds(locationIds);
	}

	return std::vector<Id>();
}

std::shared_ptr<SourceLocationCollection> StorageAccessProxy::getSourceLocationsForTokenIds(
		const std::vector<Id>& tokenIds) const
{
	if (hasSubject())
	{
		return m_subject->getSourceLocationsForTokenIds(tokenIds);
	}

	return std::make_shared<SourceLocationCollection>();
}

std::shared_ptr<SourceLocationCollection> StorageAccessProxy::getSourceLocationsForLocationIds(
		const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getSourceLocationsForLocationIds(locationIds);
	}

	return std::make_shared<SourceLocationCollection>();
}

std::shared_ptr<SourceLocationFile> StorageAccessProxy::getSourceLocationsForFile(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getSourceLocationsForFile(filePath);
	}

	return std::make_shared<SourceLocationFile>(FilePath(), false, false);
}

std::shared_ptr<SourceLocationFile> StorageAccessProxy::getSourceLocationsForLinesInFile(
	const FilePath& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getSourceLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
	}

	return std::make_shared<SourceLocationFile>(FilePath(), false, false);
}

std::shared_ptr<SourceLocationFile> StorageAccessProxy::getCommentLocationsInFile(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getCommentLocationsInFile(filePath);
	}

	return std::make_shared<SourceLocationFile>(FilePath(), false, false);
}

std::shared_ptr<TextAccess> StorageAccessProxy::getFileContent(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getFileContent(filePath);
	}

	return nullptr;
}

FileInfo StorageAccessProxy::getFileInfoForFilePath(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getFileInfoForFilePath(filePath);
	}

	return FileInfo();
}

std::vector<FileInfo> StorageAccessProxy::getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const
{
	if (hasSubject())
	{
		return m_subject->getFileInfosForFilePaths(filePaths);
	}

	return std::vector<FileInfo>();
}

StorageStats StorageAccessProxy::getStorageStats() const
{
	if (hasSubject())
	{
		return m_subject->getStorageStats();
	}

	return StorageStats();
}


ErrorCountInfo StorageAccessProxy::getErrorCount() const
{
	if (hasSubject())
	{
		return m_subject->getErrorCount();
	}

	return ErrorCountInfo();
}

std::vector<ErrorInfo> StorageAccessProxy::getErrorsLimited() const
{
	if (hasSubject())
	{
		return m_subject->getErrorsLimited();
	}

	return std::vector<ErrorInfo>();
}

std::shared_ptr<SourceLocationCollection> StorageAccessProxy::getErrorSourceLocationsLimited(std::vector<ErrorInfo>* errors) const
{
	if (hasSubject())
	{
		return m_subject->getErrorSourceLocationsLimited(errors);
	}

	return std::make_shared<SourceLocationCollection>();
}

Id StorageAccessProxy::addNodeBookmark(const NodeBookmark& bookmark)
{
	if (hasSubject())
	{
		return m_subject->addNodeBookmark(bookmark);
	}

	return -1;
}

Id StorageAccessProxy::addEdgeBookmark(const EdgeBookmark& bookmark)
{
	if (hasSubject())
	{
		return m_subject->addEdgeBookmark(bookmark);
	}

	return -1;
}

Id StorageAccessProxy::addBookmarkCategory(const std::string& categoryName)
{
	if (hasSubject())
	{
		return m_subject->addBookmarkCategory(categoryName);
	}

	return -1;
}

void StorageAccessProxy::updateBookmark(const Id bookmarkId, const std::string& name, const std::string& comment, const std::string& categoryName)
{
	if (hasSubject())
	{
		m_subject->updateBookmark(bookmarkId, name, comment, categoryName);
	}
}

void StorageAccessProxy::removeBookmark(const Id id)
{
	if (hasSubject())
	{
		m_subject->removeBookmark(id);
	}
}

void StorageAccessProxy::removeBookmarkCategory(const Id id)
{
	if (hasSubject())
	{
		m_subject->removeBookmarkCategory(id);
	}
}

std::vector<NodeBookmark> StorageAccessProxy::getAllNodeBookmarks() const
{
	if (hasSubject())
	{
		return m_subject->getAllNodeBookmarks();
	}

	return std::vector<NodeBookmark>();
}

std::vector<EdgeBookmark> StorageAccessProxy::getAllEdgeBookmarks() const
{
	if (hasSubject())
	{
		return m_subject->getAllEdgeBookmarks();
	}

	return std::vector<EdgeBookmark>();
}

std::vector<BookmarkCategory> StorageAccessProxy::getAllBookmarkCategories() const
{
	if (hasSubject())
	{
		return m_subject->getAllBookmarkCategories();
	}

	return std::vector<BookmarkCategory>();
}

void StorageAccessProxy::setErrorFilter(const ErrorFilter& filter)
{
	StorageAccess::setErrorFilter(filter);

	if (hasSubject())
	{
		m_subject->setErrorFilter(filter);
	}
}

void StorageAccessProxy::handleMessage(MessageErrorFilterChanged* message)
{
	setErrorFilter(message->errorFilter);

	if (message->showErrors)
	{
		MessageShowErrors(getErrorCount()).dispatch();
	}
}
