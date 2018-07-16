#include "BookmarkController.h"

#include "Application.h"
#include "component/view/BookmarkView.h"
#include "data/access/StorageAccess.h"
#include "data/bookmark/Bookmark.h"
#include "data/bookmark/EdgeBookmark.h"
#include "data/bookmark/NodeBookmark.h"
#include "data/storage/type/StorageEdge.h"

#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"
#include "utility/utility.h"

const std::wstring BookmarkController::s_edgeSeperatorToken = L" => ";
const std::wstring BookmarkController::s_defaultCategoryName = L"default";

BookmarkController::BookmarkController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_bookmarkCache(storageAccess)
	, m_filter(Bookmark::FILTER_ALL)
	, m_order(Bookmark::ORDER_DATE_DESCENDING)
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

void BookmarkController::displayBookmarks()
{
	getView<BookmarkView>()->displayBookmarks(getBookmarks(m_filter, m_order));
}

void BookmarkController::displayBookmarksFor(Bookmark::BookmarkFilter filter, Bookmark::BookmarkOrder order)
{
	if (filter != Bookmark::FILTER_UNKNOWN)
	{
		m_filter = filter;
	}

	if (order != Bookmark::ORDER_NONE)
	{
		m_order = order;
	}

	displayBookmarks();
}

void BookmarkController::createBookmark(
	const std::wstring& name, const std::wstring& comment, const std::wstring& category, Id nodeId
){
	LOG_INFO("Attempting to create new bookmark");

	BookmarkCategory bookmarkCategory(0, category.empty() ? s_defaultCategoryName : category);

	if (!m_activeEdgeIds.empty())
	{
		LOG_INFO("Creating Edge Bookmark");

		EdgeBookmark bookmark(0, name, comment, TimeStamp::now(), bookmarkCategory);
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
		LOG_INFO("Creating Node Bookmark");

		NodeBookmark bookmark(0, name, comment, TimeStamp::now(), bookmarkCategory);
		if (nodeId)
		{
			bookmark.addNodeId(nodeId);
		}
		else
		{
			bookmark.setNodeIds(m_activeNodeIds);
		}
		m_storageAccess->addNodeBookmark(bookmark);
	}

	m_bookmarkCache.clear();

	if (!nodeId || (m_activeNodeIds.size() == 1 && m_activeNodeIds[0] == nodeId))
	{
		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
	}

	update();
}

void BookmarkController::editBookmark(
	Id bookmarkId, const std::wstring& name, const std::wstring& comment, const std::wstring& category
){
	LOG_INFO_STREAM(<< "Attempting to update Bookmark " << bookmarkId);

	m_storageAccess->updateBookmark(bookmarkId, name, comment, category.size() ? category : s_defaultCategoryName);

	cleanBookmarkCategories();

	update();
}

void BookmarkController::deleteBookmark(Id bookmarkId)
{
	LOG_INFO_STREAM(<< "Attempting to delete Bookmark " << bookmarkId);

	m_storageAccess->removeBookmark(bookmarkId);

	cleanBookmarkCategories();

	if (!getBookmarkForActiveToken())
	{
		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
	}

	update();
}

void BookmarkController::deleteBookmarkCategory(Id categoryId)
{
	m_storageAccess->removeBookmarkCategory(categoryId);

	m_bookmarkCache.clear();

	if (!getBookmarkForActiveToken())
	{
		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
	}

	update();
}

void BookmarkController::deleteBookmarkForActiveTokens()
{
	if (std::shared_ptr<Bookmark> bookmark = getBookmarkForActiveToken())
	{
		LOG_INFO(L"Deleting bookmark " + bookmark->getName());

		m_storageAccess->removeBookmark(bookmark->getId());

		cleanBookmarkCategories();

		getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
		update();
	}
	else
	{
		LOG_WARNING("No Bookmark to delete for active tokens.");
	}
}

void BookmarkController::activateBookmark(const std::shared_ptr<Bookmark> bookmark)
{
	LOG_INFO("Attempting to activate Bookmark");

	if (std::shared_ptr<EdgeBookmark> edgeBookmark = std::dynamic_pointer_cast<EdgeBookmark>(bookmark))
	{
		if (!edgeBookmark->getEdgeIds().empty())
		{
			const Id firstEdgeId = edgeBookmark->getEdgeIds().front();
			const StorageEdge storageEdge = m_storageAccess->getEdgeById(firstEdgeId);

			const NameHierarchy sourceName = m_storageAccess->getNameHierarchyForNodeId(storageEdge.sourceNodeId);
			const NameHierarchy targetName = m_storageAccess->getNameHierarchyForNodeId(storageEdge.targetNodeId);

			if (edgeBookmark->getEdgeIds().size() == 1)
			{
				Id activeNodeId = edgeBookmark->getActiveNodeId();
				if (activeNodeId)
				{
					MessageActivateNodes(activeNodeId).dispatch();
				}

				MessageActivateEdge(firstEdgeId, Edge::intToType(storageEdge.type), sourceName, targetName).dispatch();
			}
			else
			{
				MessageActivateEdge activateEdge(0, Edge::EdgeType::EDGE_AGGREGATION, sourceName, targetName);
				for (const Id aggregatedEdgeId: edgeBookmark->getEdgeIds())
				{
					activateEdge.aggregationIds.push_back(aggregatedEdgeId);
				}
				activateEdge.dispatch();
			}
		}
		else
		{
			LOG_ERROR("Failed to activate bookmark, did not find edges to activate");
		}
	}
	else if (std::shared_ptr<NodeBookmark> nodeBookmark = std::dynamic_pointer_cast<NodeBookmark>(bookmark))
	{
		MessageActivateNodes activateNodes;

		for (Id nodeId: nodeBookmark->getNodeIds())
		{
			activateNodes.addNode(nodeId);
		}

		activateNodes.dispatch();
	}
}

void BookmarkController::showBookmarkCreator(Id nodeId)
{
	if (!canCreateBookmark() && !nodeId)
	{
		return;
	}

	BookmarkView* view = getView<BookmarkView>();

	if (nodeId)
	{
		std::shared_ptr<Bookmark> bookmark = getBookmarkForNodeId(nodeId);
		if (bookmark != nullptr)
		{
			view->displayBookmarkEditor(bookmark, getAllBookmarkCategories());
		}
		else
		{
			view->displayBookmarkCreator(getDisplayNamesForNodeId(nodeId), getAllBookmarkCategories(), nodeId);
		}
	}
	else
	{
		std::shared_ptr<Bookmark> bookmark = getBookmarkForActiveToken();
		if (bookmark != nullptr)
		{
			view->displayBookmarkEditor(bookmark, getAllBookmarkCategories());
		}
		else
		{
			view->displayBookmarkCreator(getActiveTokenDisplayNames(), getAllBookmarkCategories(), 0);
		}
	}
}

void BookmarkController::showBookmarkEditor(const std::shared_ptr<Bookmark> bookmark)
{
	getView<BookmarkView>()->displayBookmarkEditor(bookmark, getAllBookmarkCategories());
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
	activateBookmark(message->bookmark);
}

void BookmarkController::handleMessage(MessageActivateErrors* message)
{
	clear();
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

void BookmarkController::handleMessage(MessageDisplayBookmarks* message)
{
	displayBookmarksFor(message->filter, message->order);
}

void BookmarkController::handleMessage(MessageDisplayBookmarkCreator* message)
{
	showBookmarkCreator(message->nodeId);
}

void BookmarkController::handleMessage(MessageIndexingFinished* message)
{
	m_bookmarkCache.clear();
	getView<BookmarkView>()->enableDisplayBookmarks(true);

	update();
}

std::vector<std::wstring> BookmarkController::getActiveTokenDisplayNames() const
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

std::vector<std::wstring> BookmarkController::getDisplayNamesForNodeId(Id nodeId) const
{
	return std::vector<std::wstring>({ getNodeDisplayName(nodeId) });
}

std::vector<BookmarkCategory> BookmarkController::getAllBookmarkCategories() const
{
	return m_storageAccess->getAllBookmarkCategories();
}

std::shared_ptr<Bookmark> BookmarkController::getBookmarkForActiveToken() const
{
	if (!m_activeEdgeIds.empty())
	{
		for (const std::shared_ptr<EdgeBookmark>& edgeBookmark: getAllEdgeBookmarks())
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
		for (const std::shared_ptr<NodeBookmark>& nodeBookmark: getAllNodeBookmarks())
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
	for (const std::shared_ptr<NodeBookmark>& nodeBookmark: getAllNodeBookmarks())
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

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getAllBookmarks() const
{
	LOG_INFO("Retrieving all bookmarks");

	std::vector<std::shared_ptr<Bookmark>> bookmarks;

	for (const std::shared_ptr<NodeBookmark>& nodeBookmark: getAllNodeBookmarks())
	{
		bookmarks.push_back(nodeBookmark);
	}
	for (const std::shared_ptr<EdgeBookmark>& edgeBookmark: getAllEdgeBookmarks())
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

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getBookmarks(
	Bookmark::BookmarkFilter filter, Bookmark::BookmarkOrder order
) const {
	LOG_INFO_STREAM(<< "Retrieving bookmarks with filter \"" << filter << "\" and order \"" << order << "\"");

	std::vector<std::shared_ptr<Bookmark>> bookmarks = getAllBookmarks();
	bookmarks = getFilteredBookmarks(bookmarks, filter);
	bookmarks = getOrderedBookmarks(bookmarks, order);
	return bookmarks;
}

std::vector<std::wstring> BookmarkController::getActiveNodeDisplayNames() const
{
	std::vector<std::wstring> names;
	for (Id nodeId : m_activeNodeIds)
	{
		names.push_back(getNodeDisplayName(nodeId));
	}
	return names;
}

std::vector<std::wstring> BookmarkController::getActiveEdgeDisplayNames() const
{
	std::vector<std::wstring> activeEdgeDisplayNames;
	for (Id activeEdgeId: m_activeEdgeIds)
	{
		const StorageEdge activeEdge = m_storageAccess->getEdgeById(activeEdgeId);
		const std::wstring sourceDisplayName = getNodeDisplayName(activeEdge.sourceNodeId);
		const std::wstring targetDisplayName = getNodeDisplayName(activeEdge.targetNodeId);
		activeEdgeDisplayNames.push_back(sourceDisplayName + s_edgeSeperatorToken + targetDisplayName);
	}
	return activeEdgeDisplayNames;
}

std::wstring BookmarkController::getNodeDisplayName(const Id nodeId) const
{
	NodeType type = m_storageAccess->getNodeTypeForNodeWithId(nodeId);
	NameHierarchy nameHierarchy = m_storageAccess->getNameHierarchyForNodeId(nodeId);

	if (type.isFile())
	{
		return FilePath(nameHierarchy.getQualifiedName()).fileName();
	}

	return nameHierarchy.getQualifiedName();
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getFilteredBookmarks(
	const std::vector<std::shared_ptr<Bookmark>>& bookmarks, Bookmark::BookmarkFilter filter) const
{
	std::vector<std::shared_ptr<Bookmark>> result;

	if (filter == Bookmark::FILTER_ALL)
	{
		return bookmarks;
	}
	else if (filter == Bookmark::FILTER_NODES)
	{
		for (const std::shared_ptr<Bookmark>& bookmark: bookmarks)
		{
			if (std::dynamic_pointer_cast<NodeBookmark>(bookmark))
			{
				result.push_back(bookmark);
			}
		}
	}
	else if (filter == Bookmark::FILTER_EDGES)
	{
		for (const std::shared_ptr<Bookmark>& bookmark: bookmarks)
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
	const std::vector<std::shared_ptr<Bookmark>>& bookmarks, Bookmark::BookmarkOrder order) const
{
	std::vector<std::shared_ptr<Bookmark>> result = bookmarks;

	if (order == Bookmark::ORDER_DATE_ASCENDING)
	{
		return getDateOrderedBookmarks(result, true);
	}
	else if (order == Bookmark::ORDER_DATE_DESCENDING)
	{
		return getDateOrderedBookmarks(result, false);
	}
	else if (order == Bookmark::ORDER_NAME_ASCENDING)
	{
		return getNameOrderedBookmarks(result, true);
	}
	else if (order == Bookmark::ORDER_NAME_DESCENDING)
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
	std::wstring aName = a->getName();
	std::wstring bName = b->getName();

	aName = utility::toLowerCase(aName);
	bName = utility::toLowerCase(bName);

	unsigned int i = 0;
	while (i < aName.length() && i < bName.length())
	{
		if (std::towlower(aName[i]) < std::towlower(bName[i]))
		{
			return true;
		}
		else if (std::towlower(aName[i]) > std::towlower(bName[i]))
		{
			return false;
		}

		++i;
	}

	return aName.length() < bName.length();
}

void BookmarkController::update()
{
	BookmarkView* view = getView<BookmarkView>();
	if (view->bookmarkBrowserIsVisible())
	{
		view->displayBookmarks(getBookmarks(m_filter, m_order));
	}

	std::vector<std::shared_ptr<Bookmark>> bookmarks = getBookmarks(Bookmark::FILTER_ALL, Bookmark::ORDER_DATE_DESCENDING);

	const size_t maxBookmarkMenuCount = 20;
	if (bookmarks.size() > maxBookmarkMenuCount)
	{
		bookmarks.resize(maxBookmarkMenuCount);
	}

	Application::getInstance()->updateBookmarks(bookmarks);
}
