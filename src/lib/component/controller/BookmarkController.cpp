#include "BookmarkController.h"

#include "component/view/BookmarkView.h"

#include "data/access/StorageAccess.h"
#include "data/bookmark/Bookmark.h"

#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateNodes.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

#include "data/bookmark/EdgeBookmark.h"
#include "data/bookmark/NodeBookmark.h"

const std::string BookmarkController::m_edgeSeperatorToken = "=>";

BookmarkController::BookmarkController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_activeTokens()
	, m_activeTokenNames()
	, m_activeTokenDisplayNames()
	, m_activeTokenTypes()
	, m_activeEdges()
	, m_activeEdgeNames()
	, m_activeEdgeDisplayNames()
	, m_activeEdgeTypes()
	, m_activeTokenExists(false)
{
}

BookmarkController::~BookmarkController()
{
}

void BookmarkController::clear()
{
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getAllBookmarks() const
{
	LOG_INFO_STREAM(<< "Retrieving all bookmarks");

	std::vector<std::shared_ptr<Bookmark>> bookmarks;

	std::vector<NodeBookmark> nodeBookmarks = m_storageAccess->getAllNodeBookmarks();

	for (unsigned int i = 0; i < nodeBookmarks.size(); i++)
	{
		bookmarks.push_back(std::make_shared<NodeBookmark>(nodeBookmarks[i]));
	}

	std::vector<EdgeBookmark> edgeBookmarks = m_storageAccess->getAllEdgeBookmarks();

	for (unsigned int i = 0; i < edgeBookmarks.size(); i++)
	{
		bookmarks.push_back(std::make_shared<EdgeBookmark>(edgeBookmarks[i]));
	}

	// std::vector<Bookmark> bookmarks = m_storageAccess->getAllBookmarks();

	// check whether bookmarks are still valid (so, no referenced tokens have been removed)
	for (unsigned int i = 0; i < bookmarks.size(); i++)
	{
		for (unsigned int j = 0; j < bookmarks[i]->getTokenNames().size(); j++)
		{
			std::string name = bookmarks[i]->getTokenNames()[j];
			int type = bookmarks[i]->getTokenTypes()[j];

			bool exists = false;

			if (dynamic_cast<EdgeBookmark*>(bookmarks[i].get()) != NULL)
			{
				TempEdge tmpEdge = getEdge(name, type);

				exists = m_storageAccess->checkEdgeExists(tmpEdge.edgeId);
			}
			else
			{
				exists = m_storageAccess->checkNodeExistsByName(name);
			}

			std::vector<Id> tokenIds = bookmarks[i]->getTokenIds();

			bookmarks[i]->setValid(exists);
		}
	}



	return bookmarks;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getBookmarks(const MessageDisplayBookmarks::BookmarkFilter& filter, const MessageDisplayBookmarks::BookmarkOrder& order) const
{
	LOG_INFO_STREAM(<< "Retrieving bookmarks with filter '" << std::to_string(filter) << "' and order '" << std::to_string(order) << "'");

	std::vector<std::shared_ptr<Bookmark>> bookmarks = getAllBookmarks();

	int bookmarkCount = bookmarks.size();
	bookmarks = getFilteredBookmarks(bookmarks, filter);

	bookmarkCount = bookmarks.size();
	bookmarks = getOrderedBookmarks(bookmarks, order);

	return bookmarks;
}

std::vector<std::string> BookmarkController::getActiveTokenNames() const
{
	return m_activeTokenNames;
}

std::vector<std::string> BookmarkController::getActiveTokenDisplayNames() const
{
	if (m_activeEdgeDisplayNames.size() > 0)
	{
		return m_activeEdgeDisplayNames;
	}
	else
	{
		return m_activeTokenDisplayNames;
	}
}

std::vector<BookmarkCategory> BookmarkController::getAllBookmarkCategories() const
{
	return m_storageAccess->getAllBookmarkCategories();
}

bool BookmarkController::activeTokenExists() const
{
	return m_activeTokenExists;
}

std::shared_ptr<Bookmark> BookmarkController::getBookmarkForActiveToken() const
{
	if (m_activeEdgeNames.size() > 0)
	{
		std::vector<EdgeBookmark> bookmarks = m_storageAccess->getAllEdgeBookmarks();

		for (unsigned int i = 0; i < bookmarks.size(); i++)
		{
			if (bookmarks[i].getTokenNames() == m_activeEdgeNames)
			{
				return std::make_shared<EdgeBookmark>(bookmarks[i]);
			}
		}
	}
	else
	{
		std::vector<NodeBookmark> bookmarks = m_storageAccess->getAllNodeBookmarks();

		for (unsigned int i = 0; i < bookmarks.size(); i++)
		{
			if (bookmarks[i].getTokenNames() == m_activeTokenNames)
			{
				return std::make_shared<NodeBookmark>(bookmarks[i]);
			}
		}
	}

	return NULL;
}

void BookmarkController::handleMessage(MessageActivateBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to activate Bookmark");

	if (dynamic_cast<EdgeBookmark*>(message->bookmark.get()) != NULL)
	{
		EdgeBookmark* bookmark = dynamic_cast<EdgeBookmark*>(message->bookmark.get());

		NodeBookmark baseBookmark = bookmark->getBaseBookmark();

		MessageActivateNodes activateNodes;

		for (unsigned int i = 0; i < baseBookmark.getTokenNames().size(); i++)
		{
			NameHierarchy nh = NameHierarchy::deserialize(baseBookmark.getTokenNames()[i]);
			activateNodes.addNode(0, nh);
		}

		activateNodes.dispatch();

		NameHierarchy source;
		NameHierarchy target;
		for (unsigned int i = 0; i < bookmark->getTokenNames().size(); i++)
		{
			int tokenType = bookmark->getTokenTypes()[i];
			TempEdge tmpEdge = getEdge(bookmark->getTokenNames()[i], tokenType);

			MessageActivateEdge activateEdge(tmpEdge.edgeId, Edge::intToType(tokenType), tmpEdge.source, tmpEdge.target);
			activateEdge.dispatch();
		}
	}
	else
	{
		NodeBookmark* bookmark = dynamic_cast<NodeBookmark*>(message->bookmark.get());

		MessageActivateNodes activateNodes;

		for (unsigned int i = 0; i < bookmark->getTokenNames().size(); i++)
		{
			NameHierarchy nh = NameHierarchy::deserialize(bookmark->getTokenNames()[i]);
			activateNodes.addNode(0, nh);
		}

		activateNodes.dispatch();
	}
}

void BookmarkController::handleMessage(MessageActivateTokens* message)
{
	LOG_INFO_STREAM(<< "Registering new active token");

	if (message->isEdge)
	{
		LOG_INFO_STREAM(<< "Registering new Edge");

		std::vector<std::string> tokenNames;
		std::vector<std::string> tokenDisplayNames;
		std::vector<int> tokenTypes;

		for (unsigned int i = 0; i < message->tokenIds.size(); i++)
		{
			StorageEdge edge = m_storageAccess->getEdgeById(message->tokenIds[i]);

			int typeId = edge.type;

			tokenTypes.push_back(typeId);

			NameHierarchy sourceHierarchy = m_storageAccess->getNameHierarchyForNodeId(edge.sourceNodeId);
			NameHierarchy targetHierarchy = m_storageAccess->getNameHierarchyForNodeId(edge.targetNodeId);

			std::string sourceNode = NameHierarchy::serialize(sourceHierarchy);
			std::string targetNode = NameHierarchy::serialize(targetHierarchy);

			std::string tokenName = sourceNode + m_edgeSeperatorToken + targetNode;

			tokenNames.push_back(tokenName);

			tokenDisplayNames.push_back(sourceHierarchy.getRawName() + m_edgeSeperatorToken + targetHierarchy.getRawName());
		}

		m_activeEdges = message->tokenIds;
		m_activeEdgeNames = tokenNames;
		m_activeEdgeTypes = tokenTypes;
		m_activeEdgeDisplayNames = tokenDisplayNames;

		if (m_storageAccess->checkEdgeBookmarkExistsByTokens(tokenNames))
		{
			m_activeTokenExists = true;
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
		}
		else
		{
			m_activeTokenExists = false;
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
		}
	}
	else
	{
		LOG_INFO_STREAM(<< "Registering new Node");

		m_activeTokens = message->tokenIds;
		m_activeTokenNames = getTokenNames(m_activeTokens);
		m_activeTokenTypes = getTokenTypes(m_activeTokens);
		m_activeTokenDisplayNames = getTokenDisplayNames(m_activeTokens);

		m_activeEdges.clear();
		m_activeEdgeNames.clear();
		m_activeEdgeTypes.clear();
		m_activeEdgeDisplayNames.clear();

		if (m_storageAccess->checkNodeBookmarkExistsByTokens(m_activeTokenNames))
		{
			m_activeTokenExists = true;
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
		}
		else
		{
			m_activeTokenExists = false;
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::CAN_CREATE);
		}
	}
}

void BookmarkController::handleMessage(MessageCreateBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to create new bookmark");

	if (m_activeTokens.size() > 0)
	{
		if (m_activeEdges.size() > 0)
		{
			LOG_INFO_STREAM(<< "Creating Edge Bookmark");

			std::string displayName = message->displayName;
			if (displayName.size() <= 0)
			{
				displayName = getDisplayName(m_activeTokens[0]);
			}

			EdgeBookmark bookmark(displayName, m_activeTokens, m_activeTokenNames, message->comment, TimePoint::now());

			bookmark.setDisplayName(message->displayName);
			bookmark.setComment(message->comment);
			bookmark.setTokenTypes(m_activeTokenTypes);

			BookmarkCategory category;
			category.setName(message->categoryName);

			bookmark.setCategory(category);

			bookmark.setEdgeTokenIds(m_activeEdges);
			bookmark.setEdgeTokenNames(m_activeEdgeNames);
			bookmark.setEdgeTokenTypes(m_activeEdgeTypes);

			Id id = m_storageAccess->addEdgeBookmark(bookmark);

			bookmark.setId(id);

			m_activeTokenExists = true;
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
			getView<BookmarkView>()->update();
		}
		else
		{
			LOG_INFO_STREAM(<< "Creating Node Bookmark");

			std::string displayName = message->displayName;
			if (displayName.size() <= 0)
			{
				displayName = getDisplayName(m_activeTokens[0]);
			}

			NodeBookmark bookmark(displayName, m_activeTokens, m_activeTokenNames, message->comment, TimePoint::now());

			bookmark.setTokenTypes(m_activeTokenTypes);

			BookmarkCategory category;
			category.setName(message->categoryName);

			bookmark.setCategory(category);

			Id id = m_storageAccess->addNodeBookmark(bookmark);

			bookmark.setId(id);

			m_activeTokenExists = true;
			getView<BookmarkView>()->setCreateButtonState(BookmarkView::CreateButtonState::ALREADY_CREATED);
			getView<BookmarkView>()->update();
		}
	}
}

void BookmarkController::handleMessage(MessageCreateBookmarkCategory* message)
{
	std::string categoryName = message->name;
	LOG_INFO_STREAM(<< "Attempting to create new Bookmark category '" << categoryName << "'");

	if (m_storageAccess->checkBookmarkCategoryExists(message->name) == false)
	{
		BookmarkCategory category;
		category.setName(message->name);

		m_storageAccess->addBookmarkCategory(category);
	}
}

void BookmarkController::handleMessage(MessageDeleteBookmark* message)
{
	LOG_INFO_STREAM(<< "Attempting to delete Bookmark " << std::to_string(message->bookmarkId));

	if (message->isEdge)
	{
		m_storageAccess->removeEdgeBookmark(message->bookmarkId);
	}
	else
	{
		m_storageAccess->removeNodeBookmark(message->bookmarkId);
	}

	cleanBookmarkCategories();

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageDeleteBookmarkCategoryWithBookmarks* message)
{
	std::vector<BookmarkCategory> categories = m_storageAccess->getAllBookmarkCategories();

	for (unsigned int i = 0; i < categories.size(); i++)
	{
		if (categories[i].getId() == message->categoryId)
		{
			std::vector<NodeBookmark> nodeBookmarks = m_storageAccess->getAllNodeBookmarks();
			for (unsigned int j = 0; j < nodeBookmarks.size(); j++)
			{
				if (nodeBookmarks[j].getCategory().getName() == categories[i].getName())
				{
					m_storageAccess->removeNodeBookmark(nodeBookmarks[j].getId());
				}
			}

			std::vector<EdgeBookmark> edgeBookmarks = m_storageAccess->getAllEdgeBookmarks();
			for (unsigned int j = 0; j < edgeBookmarks.size(); j++)
			{
				if (edgeBookmarks[j].getCategory().getName() == categories[i].getName())
				{
					m_storageAccess->removeEdgeBookmark(edgeBookmarks[j].getId());
				}
			}

			cleanBookmarkCategories();
			getView<BookmarkView>()->update();

			return;
		}
	}
}

void BookmarkController::handleMessage(MessageDeleteBookmarkForActiveTokens* message)
{
	std::shared_ptr<Bookmark> bookmark = getBookmarkForActiveToken();

	if (bookmark != NULL)
	{
		LOG_INFO_STREAM(<< "Deleting bookmark " << bookmark->getDisplayName());

		if (dynamic_cast<EdgeBookmark*>(bookmark.get()) != NULL)
		{
			m_storageAccess->removeEdgeBookmark(bookmark->getId());
		}
		else if(dynamic_cast<NodeBookmark*>(bookmark.get()) != NULL)
		{
			m_storageAccess->removeNodeBookmark(bookmark->getId());
		}

		cleanBookmarkCategories();

		m_activeTokenExists = false;
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

	if (message->isEdge)
	{
		EdgeBookmark bookmark = m_storageAccess->getEdgeBookmarkById(message->bookmarkId);

		bookmark.setDisplayName(message->displayName);
		bookmark.setComment(message->comment);
		BookmarkCategory category;
		category.setName(message->categoryName);
		bookmark.setCategory(category);

		m_storageAccess->editEdgeBookmark(bookmark);
	}
	else
	{
		NodeBookmark bookmark = m_storageAccess->getNodeBookmarkById(message->bookmarkId);

		bookmark.setDisplayName(message->displayName);
		bookmark.setComment(message->comment);
		BookmarkCategory category;
		category.setName(message->categoryName);
		bookmark.setCategory(category);

		m_storageAccess->editNodeBookmark(bookmark);
	}

	getView<BookmarkView>()->update();
}

void BookmarkController::handleMessage(MessageFinishedParsing* message)
{
	getView<BookmarkView>()->enableDisplayBookmarks(true);
}

std::vector<std::string> BookmarkController::getTokenNames(const std::vector<Id>& ids) const
{
	std::vector<std::string> names;

	for (unsigned int i = 0; i < ids.size(); i++)
	{
		NameHierarchy nameHierarchy = m_storageAccess->getNameHierarchyForNodeId(ids[i]);
		names.push_back(NameHierarchy::serialize(nameHierarchy));
	}

	return names;
}

std::vector<std::string> BookmarkController::getTokenDisplayNames(const std::vector<Id>& ids) const
{
	std::vector<std::string> names;

	for (unsigned int i = 0; i < ids.size(); i++)
	{
		NameHierarchy nameHierarchy = m_storageAccess->getNameHierarchyForNodeId(ids[i]);
		names.push_back(nameHierarchy.getRawName());
	}

	return names;
}

std::vector<int> BookmarkController::getTokenTypes(const std::vector<Id>& ids) const
{
	std::vector<int> types;

	for (unsigned int i = 0; i < ids.size(); i++)
	{
		int type = m_storageAccess->getNodeTypeForNodeWithId(ids[i]);
		types.push_back(type);
	}

	return types;
}

std::string BookmarkController::getDisplayName(Id id) const
{
	NameHierarchy nameHierarchy = m_storageAccess->getNameHierarchyForNodeId(id);
	return nameHierarchy.getRawName();
}

BookmarkController::TempEdge BookmarkController::getEdge(const std::string& tokenName, const int tokenType) const
{
	std::pair<std::string, std::string> edgeTokens = seperateEdgeToken(tokenName);

	NameHierarchy source = NameHierarchy::deserialize(edgeTokens.first);
	NameHierarchy target = NameHierarchy::deserialize(edgeTokens.second);

	Id edgeId = m_storageAccess->getIdForEdge(Edge::intToType(tokenType), source, target);

	TempEdge result;

	result.edgeId = edgeId;
	result.source = source;
	result.target = target;
	result.type = tokenType;

	return result;
}

std::pair<std::string, std::string> BookmarkController::seperateEdgeToken(const std::string& token) const
{
	std::pair<std::string, std::string> result;

	if (token.find(m_edgeSeperatorToken) != std::string::npos)
	{
		std::vector<std::string> st = utility::splitToVector(token, m_edgeSeperatorToken);

		if (st.size() != 2)
		{
			LOG_ERROR_STREAM(<< "Invalid edge token found: " << token);

			return result;
		}
		else
		{
			result.first = st[0];
			result.second = st[1];
		}
	}

	return result;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getFilteredBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkFilter& filter) const
{
	std::vector<std::shared_ptr<Bookmark>> result;

	if (filter == MessageDisplayBookmarks::BookmarkFilter::ALL)
	{
		return bookmarks;
	}
	else if (filter == MessageDisplayBookmarks::BookmarkFilter::NODES)
	{
		for (unsigned int i = 0; i < bookmarks.size(); i++)
		{
			if (dynamic_cast<EdgeBookmark*>(bookmarks[i].get()) == NULL)
			{
				result.push_back(bookmarks[i]);
			}
		}

		return result;
	}
	else if (filter == MessageDisplayBookmarks::BookmarkFilter::EDGES)
	{
		for (unsigned int i = 0; i < bookmarks.size(); i++)
		{
			if (dynamic_cast<EdgeBookmark*>(bookmarks[i].get()) != NULL)
			{
				result.push_back(bookmarks[i]);
			}
		}

		return result;
	}

	return result;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getOrderedBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkOrder& order) const
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

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getDateOrderedBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const
{
	std::vector<std::shared_ptr<Bookmark>> result = bookmarks;

	std::sort(result.begin(), result.end(), BookmarkController::bookmarkDateCompare);

	if (ascending == false)
	{
		std::reverse(result.begin(), result.end());
	}

	return result;
}

std::vector<std::shared_ptr<Bookmark>> BookmarkController::getNameOrderedBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const
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
	std::vector<std::shared_ptr<Bookmark>> bookmarks = getAllBookmarks();

	std::vector<BookmarkCategory> categories = getAllBookmarkCategories();

	for (unsigned int i = 0; i < categories.size(); i++)
	{
		BookmarkCategory category = categories[i];
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
	std::string aName = a->getDisplayName();
	std::string bName = b->getDisplayName();

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