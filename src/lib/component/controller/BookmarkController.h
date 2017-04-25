#ifndef BOOKMARK_CONTROLLER_H
#define BOOKMARK_CONTROLLER_H

#include "data/bookmark/Bookmark.h"
#include "data/bookmark/NodeBookmark.h"
#include "data/bookmark/EdgeBookmark.h"
#include "data/name/NameHierarchy.h"
#include "data/StorageTypes.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateBookmark.h"
#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageCreateBookmark.h"
#include "utility/messaging/type/MessageCreateBookmarkCategory.h"
#include "utility/messaging/type/MessageDeleteBookmark.h"
#include "utility/messaging/type/MessageDeleteBookmarkCategory.h"
#include "utility/messaging/type/MessageDeleteBookmarkForActiveTokens.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageEditBookmark.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

#include "component/controller/Controller.h"

class StorageAccess;

class BookmarkController
	: public Controller
	, public MessageListener<MessageActivateBookmark>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageCreateBookmark>
	, public MessageListener<MessageCreateBookmarkCategory>
	, public MessageListener<MessageDeleteBookmark>
	, public MessageListener<MessageDeleteBookmarkCategory>
	, public MessageListener<MessageDeleteBookmarkForActiveTokens>
	, public MessageListener<MessageEditBookmark>
	, public MessageListener<MessageFinishedParsing>
{
public:
	BookmarkController(StorageAccess* storageAccess);
	virtual ~BookmarkController();

	virtual void clear();

	std::vector<std::shared_ptr<Bookmark>> getBookmarks(
		const MessageDisplayBookmarks::BookmarkFilter& filter, const MessageDisplayBookmarks::BookmarkOrder& order) const;

	std::vector<std::string> getActiveTokenDisplayNames() const;
	std::vector<BookmarkCategory> getAllBookmarkCategories() const;
	bool hasBookmarkForActiveToken() const;
	std::shared_ptr<Bookmark> getBookmarkForActiveToken() const;

private:
	class BookmarkCache
	{
	public:
		BookmarkCache(StorageAccess* storageAccess);

		void clear();

		std::vector<NodeBookmark> getAllNodeBookmarks();
		std::vector<EdgeBookmark> getAllEdgeBookmarks();

	private:
		StorageAccess* m_storageAccess;
		std::vector<NodeBookmark> m_nodeBookmarks;
		std::vector<EdgeBookmark> m_edgeBookmarks;
		bool m_nodeBookmarksValid;
		bool m_edgeBookmarksValid;
	};

	virtual void handleMessage(MessageActivateBookmark* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageCreateBookmark* message);
	virtual void handleMessage(MessageCreateBookmarkCategory* message);
	virtual void handleMessage(MessageDeleteBookmark* message);
	virtual void handleMessage(MessageDeleteBookmarkCategory* message);
	virtual void handleMessage(MessageDeleteBookmarkForActiveTokens* message);
	virtual void handleMessage(MessageEditBookmark* message);
	virtual void handleMessage(MessageFinishedParsing* message);

	std::vector<std::shared_ptr<Bookmark>> getAllBookmarks() const;
	std::vector<std::shared_ptr<NodeBookmark>> getAllNodeBookmarks() const;
	std::vector<std::shared_ptr<EdgeBookmark>> getAllEdgeBookmarks() const;

	std::vector<std::string> getActiveNodeDisplayNames() const;
	std::vector<std::string> getActiveEdgeDisplayNames() const;
	std::string getNodeDisplayName(const Id id) const;

	std::vector<std::shared_ptr<Bookmark>> getFilteredBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkFilter& filter) const;
	std::vector<std::shared_ptr<Bookmark>> getOrderedBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkOrder& order) const;
	std::vector<std::shared_ptr<Bookmark>> getDateOrderedBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const;
	std::vector<std::shared_ptr<Bookmark>> getNameOrderedBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const;

	void cleanBookmarkCategories();

	static bool bookmarkDateCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b);
	static bool bookmarkNameCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b);

	static const std::string s_edgeSeperatorToken;
	static const std::string s_defaultCategoryName;

	StorageAccess* m_storageAccess;
	mutable BookmarkCache m_bookmarkCache;

	std::vector<Id> m_activeNodeIds;
	std::vector<Id> m_activeEdgeIds;
	bool m_hasBookmarkForActiveToken;
};

#endif // BOOKMARK_CONTROLLER_H
