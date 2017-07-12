#include "BookmarkController.h"

#include "component/view/BookmarkView.h"

#include "data/access/StorageAccess.h"
#include "data/bookmark/Bookmark.h"

#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateNodes.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"
#include "utility/utility.h"

#include "data/bookmark/EdgeBookmark.h"
#include "data/bookmark/NodeBookmark.h"

const std::string BookmarkController::s_edgeSeperatorToken = " => ";
const std::string BookmarkController::s_defaultCategoryName = "default";

BookmarkController::BookmarkController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_bookmarkCache(storageAccess)
{
}

BookmarkController::~BookmarkController()
{
}

void BookmarkController::clear()
{
	m_activeNodeIds.clear();
	m_activeEdgeIds.clear();

	getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CANNOT_CREATE);
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getBookmarks(
	const MessageDisplayBookmarks::BookmarkFilter& filter, const MessageDisplayBookmarks::BookmarkOrder& order) const
{
	LOG_INFO_STREAM(<< "Retrieving bookmarks with filter \"" << std::to_string(filter) << "\" and order \"" << std::to_string(order) << "\"");

	std::vector<std::shared_ptr<Bookmark>> bookmarks = getAllBookmarks();

	bookmarks = getFilteredBookmarks(bookmarks, filter);

	bookmarks = getOrderedBookmarks(bookmarks, order);

	return bookmarks;
}

std::vector<std::string> BookmarkController::getActiveTokenDisplayNames() const
{
	if (m_activeEdgeIds.size() > 0)
	{
		return getActiveEdgeDisplayNames();
	}
	else
	{
		return getActiveNodeDisplayNames();
	}
}

std::vector<std::string> BookmarkController::getDisplayNamesForNodeId(Id nodeId) const
{
	return std::vector<std::string>({ getNodeDisplayName(nodeId) });
}

std::vector<BookmarkCategory> BookmarkController::getAllBookmarkCategories() const
{
	return m_storageAccess->getAllBookmarkCategories();
}

std::shared_ptr<Bookmark> BookmarkController::getBookmarkForActiveToken() const
{
	if (!m_activeEdgeIds.empty())
	{
		for (std::shared_ptr<EdgeBookmark> edgeBookmark: getAllEdgeBookmarks())
		{
			if (!m_activeNodeIds.empty() && edgeBookmark->getActiveNodeId() == m_activeNodeIds.front() &&
				utility::isPermutation(edgeBookmark->getEdgeIds(), m_activeEdgeIds))
			{
				return std::make_shared<EdgeBookmark>(*(edgeBookmark.get()));
			}
		}
	}
	else
	{
		for (std::shared_ptr<NodeBookmark> nodeBookmark: getAllNodeBookmarks())
		{
			if (utility::isPermutation(nodeBookmark->getNodeIds(), m_activeNodeIds))
			{
				return std::make_shared<NodeBookmark>(*(nodeBookmark.get()));
			}
		}
	}

	return std::shared_ptr<Bookmark>();
}

std::shared_ptr<Bookmark> BookmarkController::getBookmarkForNodeId(Id nodeId) const
{
	for (std::shared_ptr<NodeBookmark> nodeBookmark: getAllNodeBookmarks())
	{
		if (nodeBookmark->getNodeIds().size() == 1 && nodeBookmark->getNodeIds()[0] == nodeId)
		{
			return std::make_shared<NodeBookmark>(*(nodeBookmark.get()));
		}
	}

	return std::shared_ptr<Bookmark>();
}

bool BookmarkController::canCreateBookmark() const
{
	return m_activeNodeIds.size() || m_activeEdgeIds.size();
}

BookmarkController::BookmarkCache::BookmarkCache(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

void BookmarkController::BookmarkCache::clear()
{
	m_nodeBookmarksValid = false;
	m_edgeBookmarksValid = false;
}

std::vector<NodeBookmark> BookmarkController::BookmarkCache::getAllNodeBookmarks()
{
	if (!m_nodeBookmarksValid)
	{
		m_nodeBookmarks = m_storageAccess->getAllNodeBookmarks();
		m_nodeBookmarksValid = true;
	}
	return m_nodeBookmarks;
}

std::vector<EdgeBookmark> BookmarkController::BookmarkCache::getAllEdgeBookmarks()
{
	if (!m_edgeBookmarksValid)
	{
		m_edgeBookmarks = m_storageAccess->getAllEdgeBookmarks();
		m_edgeBookmarksValid = true;
	}
	return m_edgeBookmarks;
}

void BookmarkController::handleMessage(MessageActivateAll* message)
{
	clear();
}

void BookmarkController::handleMessage(MessageActivateBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to activate Bookmark");

	if (std::shared_ptr<EdgeBookmark> bookmark = std::dynamic_pointer_cast<EdgeBookmark>(message->bookmark))
	{
		if (!bookmark->getEdgeIds().empty())
		{
			const Id firstEdgeId = bookmark->getEdgeIds().front();
			const StorageEdge storageEdge = m_storageAccess->getEdgeById(firstEdgeId);

			const NameHierarchy sourceName = m_storageAccess->getNameHierarchyForNodeId(storageEdge.sourceNodeId);
			const NameHierarchy targetName = m_storageAccess->getNameHierarchyForNodeId(storageEdge.targetNodeId);

			if (bookmark->getEdgeIds().size() == 1)
			{
				Id activeNodeId = bookmark->getActiveNodeId();
				if (activeNodeId)
				{
					MessageActivateNodes activateNodes;
					activateNodes.addNode(activeNodeId, m_storageAccess->getNameHierarchyForNodeId(activeNodeId));
					activateNodes.dispatch();
				}

				MessageActivateEdge(firstEdgeId, Edge::intToType(storageEdge.type), sourceName, targetName).dispatch();
			}
			else
			{
				MessageActivateEdge activateEdge(0, Edge::EdgeType::EDGE_AGGREGATION, sourceName, targetName);
				for (const Id aggregatedEdgeId: bookmark->getEdgeIds())
				{
					activateEdge.aggregationIds.push_back(aggregatedEdgeId);
				}
				activateEdge.dispatch();
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to activate bookmark, did not find edges to activate");
		}
	}
	else if (std::shared_ptr<NodeBookmark> bookmark = std::dynamic_pointer_cast<NodeBookmark>(message->bookmark))
	{
		MessageActivateNodes activateNodes;

		for (Id nodeId: bookmark->getNodeIds())
		{
			activateNodes.addNode(nodeId, m_storageAccess->getNameHierarchyForNodeId(nodeId));
		}

		activateNodes.dispatch();
	}
}

void BookmarkController::handleMessage(MessageActivateTokens* message)
{
	m_activeEdgeIds.clear();

	if (message->isEdge || message->isAggregation)
	{
		m_activeEdgeIds = message->tokenIds;

		if (getBookmarkForActiveToken())
		{
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
		}
		else
		{
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
		}
	}
	else if (!message->isEdge)
	{
		m_activeNodeIds = message->tokenIds;

		if (getBookmarkForActiveToken())
		{
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
		}
		else
		{
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
		}
	}
}

void BookmarkController::handleMessage(MessageCreateBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to create new bookmark");

	BookmarkCategory category(0, message->categoryName.empty() ? s_defaultCategoryName : message->categoryName);

	if (!m_activeEdgeIds.empty())
	{
		LOG_INFO_STREAM(<< "Creating Edge Bookmark");

		EdgeBookmark bookmark(0, message->displayName, message->comment, TimePoint::now(), category);
		bookmark.setEdgeIds(m_activeEdgeIds);

		if (!m_activeNodeIds.empty())
		{
			bookmark.setActiveNodeId(m_activeNodeIds.front());
		}
		else
		{
			LOG_ERROR("Cannot create bookmark for edge if no active node exists");
		}

		m_storageAccess->addEdgeBookmark(bookmark);
	}
	else
	{
		LOG_INFO_STREAM(<< "Creating Node Bookmark");

		NodeBookmark bookmark(0, message->displayName, message->comment, TimePoint::now(), category);
		if (message->nodeId)
		{
			bookmark.addNodeId(message->nodeId);
		}
		else
		{
			bookmark.setNodeIds(m_activeNodeIds);
		}
		m_storageAccess->addNodeBookmark(bookmark);
	}

	m_bookmarkCache.clear();

	if (!message->nodeId || (m_activeNodeIds.size() == 1 && m_activeNodeIds[0] == message->nodeId))
	{
		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
	}

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageCreateBookmarkCategory* message)
{
	const std::string& categoryName = message->name.empty() ? s_defaultCategoryName : message->name;
	LOG_INFO_STREAM(<< "Attempting to create new Bookmark category \"" << categoryName << "\"");
	m_storageAccess->addBookmarkCategory(categoryName);
}

void BookmarkController::handleMessage(MessageDeleteBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to delete Bookmark " << std::to_string(message->bookmarkId));

	m_storageAccess->removeBookmark(message->bookmarkId);

	cleanBookmarkCategories();

	if (!getBookmarkForActiveToken())
	{
		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
	}

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageDeleteBookmarkCategory* message)
{
	m_storageAccess->removeBookmarkCategory(message->categoryId);

	m_bookmarkCache.clear();

	if (!getBookmarkForActiveToken())
	{
		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
	}

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageDeleteBookmarkForActiveTokens* message)
{
	if (std::shared_ptr<Bookmark> bookmark = getBookmarkForActiveToken())
	{
		LOG_INFO_STREAM(<< "Deleting bookmark " << bookmark->getName());

		m_storageAccess->removeBookmark(bookmark->getId());

		cleanBookmarkCategories();

		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
		getView<BookmarkView>()->update();
	}
	else
	{
		LOG_WARNING_STREAM(<< "No Bookmark to delete for active tokens.");
	}
}

void BookmarkController::handleMessage(MessageEditBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to update Bookmark " << std::to_string(message->bookmarkId));

	const std::string& categoryName = message->categoryName.empty() ? s_defaultCategoryName : message->categoryName;
	m_storageAccess->updateBookmark(message->bookmarkId, message->displayName, message->comment, categoryName);

	cleanBookmarkCategories();

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageFinishedParsing* message)
{
	m_bookmarkCache.clear();
	getView<BookmarkView>()->enableDisplayBookmarks(true);

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageShowErrors* message)
{
	clear();
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getAllBookmarks() const
{
	LOG_INFO_STREAM(<< "Retrieving all bookmarks");

	std::vector<std::shared_ptr<Bookmark>> bookmarks;

	for (std::shared_ptr<NodeBookmark> nodeBookmark: getAllNodeBookmarks())
	{
		bookmarks.push_back(nodeBookmark);
	}
	for (std::shared_ptr<EdgeBookmark> edgeBookmark: getAllEdgeBookmarks())
	{
		bookmarks.push_back(edgeBookmark);
	}

	return bookmarks;
}

std::vector<std::shared_ptr<NodeBookmark>> BookmarkController::getAllNodeBookmarks() const
{
	std::vector<std::shared_ptr<NodeBookmark>> bookmarks;
	for (const NodeBookmark& nodeBookmark: m_bookmarkCache.getAllNodeBookmarks())
	{
		bookmarks.push_back(std::make_shared<NodeBookmark>(nodeBookmark));
	}
	return bookmarks;
}

std::vector<std::shared_ptr<EdgeBookmark>> BookmarkController::getAllEdgeBookmarks() const
{
	std::vector<std::shared_ptr<EdgeBookmark>> bookmarks;
	for (const EdgeBookmark& edgeBookmark: m_bookmarkCache.getAllEdgeBookmarks())
	{
		bookmarks.push_back(std::make_shared<EdgeBookmark>(edgeBookmark));
	}
	return bookmarks;
}

std::vector<std::string> BookmarkController::getActiveNodeDisplayNames() const
{
	std::vector<std::string> names;
	for (Id nodeId : m_activeNodeIds)
	{
		names.push_back(getNodeDisplayName(nodeId));
	}
	return names;
}

std::vector<std::string> BookmarkController::getActiveEdgeDisplayNames() const
{
	std::vector<std::string> activeEdgeDisplayNames;
	for (Id activeEdgeId: m_activeEdgeIds)
	{
		const StorageEdge activeEdge = m_storageAccess->getEdgeById(activeEdgeId);
		const std::string sourceDisplayName = getNodeDisplayName(activeEdge.sourceNodeId);
		const std::string targetDisplayName = getNodeDisplayName(activeEdge.targetNodeId);
		activeEdgeDisplayNames.push_back(sourceDisplayName + s_edgeSeperatorToken + targetDisplayName);
	}
	return activeEdgeDisplayNames;
}

std::string BookmarkController::getNodeDisplayName(const Id nodeId) const
{
	Node::NodeType type = m_storageAccess->getNodeTypeForNodeWithId(nodeId);
	NameHierarchy nameHierarchy = m_storageAccess->getNameHierarchyForNodeId(nodeId);

	if (type == Node::NODE_FILE)
	{
		return FilePath(nameHierarchy.getQualifiedName()).fileName();
	}

	return nameHierarchy.getQualifiedName();
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getFilteredBookmarks(
	const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkFilter& filter) const
{
	std::vector<std::shared_ptr<Bookmark>> result;

	if (filter == MessageDisplayBookmarks::BookmarkFilter::ALL)
	{
		return bookmarks;
	}
	else if (filter == MessageDisplayBookmarks::BookmarkFilter::NODES)
	{
		for (std::shared_ptr<Bookmark> bookmark: bookmarks)
		{
			if (std::dynamic_pointer_cast<NodeBookmark>(bookmark))
			{
				result.push_back(bookmark);
			}
		}
	}
	else if (filter == MessageDisplayBookmarks::BookmarkFilter::EDGES)
	{
		for (std::shared_ptr<Bookmark> bookmark: bookmarks)
		{
			if (std::dynamic_pointer_cast<EdgeBookmark>(bookmark))
			{
				result.push_back(bookmark);
			}
		}
	}

	return result;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getOrderedBookmarks(
	const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkOrder& order) const
{
	std::vector<std::shared_ptr<Bookmark>> result = bookmarks;

	if (order == MessageDisplayBookmarks::BookmarkOrder::DATE_ASCENDING)
	{
		return getDateOrderedBookmarks(result, true);
	}
	else if (order == MessageDisplayBookmarks::BookmarkOrder::DATE_DESCENDING)
	{
		return getDateOrderedBookmarks(result, false);
	}
	else if (order == MessageDisplayBookmarks::BookmarkOrder::NAME_ASCENDING)
	{
		return getNameOrderedBookmarks(result, true);
	}
	else if (order == MessageDisplayBookmarks::BookmarkOrder::NAME_DESCENDING)
	{
		return getNameOrderedBookmarks(result, false);
	}

	return result;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getDateOrderedBookmarks(
	const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const
{
	std::vector<std::shared_ptr<Bookmark>> result = bookmarks;

	std::sort(result.begin(), result.end(), BookmarkController::bookmarkDateCompare);

	if (ascending == false)
	{
		std::reverse(result.begin(), result.end());
	}

	return result;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getNameOrderedBookmarks(
	const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const
{
	std::vector<std::shared_ptr<Bookmark>> result = bookmarks;

	std::sort(result.begin(), result.end(), BookmarkController::bookmarkNameCompare);

	if (ascending == false)
	{
		std::reverse(result.begin(), result.end());
	}

	return result;
}

void BookmarkController::cleanBookmarkCategories()
{
	m_bookmarkCache.clear();

	std::vector<std::shared_ptr<Bookmark>> bookmarks = getAllBookmarks();

	for (const BookmarkCategory& category: getAllBookmarkCategories())
	{
		bool used = false;

		for (unsigned int j = 0; j < bookmarks.size(); j++)
		{
			if (bookmarks[j]->getCategory().getName() == category.getName())
			{
				used = true;
				break;
			}
		}

		if (used == false)
		{
			m_storageAccess->removeBookmarkCategory(category.getId());
		}
	}
}

bool BookmarkController::bookmarkDateCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b)
{
	return a->getTimeStamp() < b->getTimeStamp();
}

bool BookmarkController::bookmarkNameCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b)
{
	std::string aName = a->getName();
	std::string bName = b->getName();

	aName = utility::toLowerCase(aName);
	bName = utility::toLowerCase(bName);

	unsigned int i = 0;
	while (i < aName.length() && i < bName.length())
	{
		if (std::tolower(aName[i]) < std::tolower(bName[i]))
		{
			return true;
		}
		else if (std::tolower(aName[i]) > std::tolower(bName[i]))
		{
			return false;
		}

		++i;
	}

	return aName.length() < bName.length();
}
