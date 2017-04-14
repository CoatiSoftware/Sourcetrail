#include "data/access/StorageAccessProxy.h"

#include "data/graph/Graph.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"

#include "utility/logging/logging.h"
#include "utility/file/FileInfo.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/TimePoint.h"

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

	return NameHierarchy();
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

bool StorageAccessProxy::checkNodeExistsByName(const std::string& serializedName) const
{
	if (hasSubject())
	{
		return m_subject->checkNodeExistsByName(serializedName);
	}
	return false;
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

bool StorageAccessProxy::checkEdgeExists(Id edgeId) const
{
	if (hasSubject())
	{
		return m_subject->checkEdgeExists(edgeId);
	}

	return false;
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

std::shared_ptr<Graph> StorageAccessProxy::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds, bool* isActiveNamespace) const
{
	if (hasSubject())
	{
		return m_subject->getGraphForActiveTokenIds(tokenIds, isActiveNamespace);
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

	return std::make_shared<SourceLocationFile>("", false, false);
}

std::shared_ptr<SourceLocationFile> StorageAccessProxy::getSourceLocationsForLinesInFile(
	const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getSourceLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
	}

	return std::make_shared<SourceLocationFile>("", false, false);
}

std::shared_ptr<SourceLocationFile> StorageAccessProxy::getCommentLocationsInFile(const FilePath& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getCommentLocationsInFile(filePath);
	}

	return std::make_shared<SourceLocationFile>("", false, false);
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

std::vector<ErrorInfo> StorageAccessProxy::getErrors() const
{
	if (hasSubject())
	{
		return m_subject->getErrors();;
	}

	return std::vector<ErrorInfo>();
}

std::shared_ptr<SourceLocationCollection> StorageAccessProxy::getErrorSourceLocations(std::vector<ErrorInfo>* errors) const
{
	if (hasSubject())
	{
		return m_subject->getErrorSourceLocations(errors);
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

Id StorageAccessProxy::addBookmarkCategory(const BookmarkCategory& category)
{
	if (hasSubject())
	{
		return m_subject->addBookmarkCategory(category);
	}

	return -1;
}

std::vector<NodeBookmark> StorageAccessProxy::getAllNodeBookmarks() const
{
	if (hasSubject())
	{
		return m_subject->getAllNodeBookmarks();
	}

	return std::vector<NodeBookmark>();
}

NodeBookmark StorageAccessProxy::getNodeBookmarkById(const Id bookmarkId) const
{
	if (hasSubject())
	{
		return m_subject->getNodeBookmarkById(bookmarkId);
	}

	return NodeBookmark();
}

bool StorageAccessProxy::checkNodeBookmarkExistsByTokens(const std::vector<std::string>& tokenNames) const
{
	if (hasSubject())
	{
		return m_subject->checkNodeBookmarkExistsByTokens(tokenNames);
	}

	return false;
}

void StorageAccessProxy::removeNodeBookmark(Id id)
{
	if (hasSubject())
	{
		m_subject->removeNodeBookmark(id);
	}
}

void StorageAccessProxy::editNodeBookmark(const NodeBookmark& bookmark)
{
	if (hasSubject())
	{
		m_subject->editNodeBookmark(bookmark);
	}
}

std::vector<EdgeBookmark> StorageAccessProxy::getAllEdgeBookmarks() const
{
	if (hasSubject())
	{
		return m_subject->getAllEdgeBookmarks();
	}

	return std::vector<EdgeBookmark>();
}

EdgeBookmark StorageAccessProxy::getEdgeBookmarkById(const Id bookmarkId) const
{
	if (hasSubject())
	{
		return m_subject->getEdgeBookmarkById(bookmarkId);
	}

	return EdgeBookmark();
}

bool StorageAccessProxy::checkEdgeBookmarkExistsByTokens(const std::vector<std::string>& tokenNames) const
{
	if (hasSubject())
	{
		return m_subject->checkEdgeBookmarkExistsByTokens(tokenNames);
	}

	return false;
}

void StorageAccessProxy::removeEdgeBookmark(Id id)
{
	if (hasSubject())
	{
		m_subject->removeEdgeBookmark(id);
	}
}

void StorageAccessProxy::editEdgeBookmark(const EdgeBookmark& bookmark)
{
	if (hasSubject())
	{
		m_subject->editEdgeBookmark(bookmark);
	}
}

std::vector<BookmarkCategory> StorageAccessProxy::getAllBookmarkCategories() const
{
	if (hasSubject())
	{
		return m_subject->getAllBookmarkCategories();
	}

	return std::vector<BookmarkCategory>();
}

bool StorageAccessProxy::checkBookmarkCategoryExists(const std::string& name) const
{
	if (hasSubject())
	{
		return m_subject->checkBookmarkCategoryExists(name);
	}

	return false;
}

void StorageAccessProxy::removeBookmarkCategory(Id id)
{
	if (hasSubject())
	{
		m_subject->removeBookmarkCategory(id);
	}
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

	MessageShowErrors(getErrorCount()).dispatch();
}
