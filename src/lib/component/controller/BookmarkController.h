#ifndef BOOKMARK_CONTROLLER_H
#define BOOKMARK_CONTROLLER_H

#include "data/bookmark/Bookmark.h"
#include "data/name/NameHierarchy.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateBookmark.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageCreateBookmark.h"
#include "utility/messaging/type/MessageCreateBookmarkCategory.h"
#include "utility/messaging/type/MessageDeleteBookmark.h"
#include "utility/messaging/type/MessageDeleteBookmarkCategoryWithBookmarks.h"
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
	, public MessageListener<MessageDeleteBookmarkCategoryWithBookmarks>
	, public MessageListener<MessageDeleteBookmarkForActiveTokens>
	, public MessageListener<MessageEditBookmark>
	, public MessageListener<MessageFinishedParsing>
{
public:
	BookmarkController(StorageAccess* storageAccess);
	virtual ~BookmarkController();

	virtual void clear();

	std::vector<std::shared_ptr<Bookmark>> getAllBookmarks() const;
	std::vector<std::shared_ptr<Bookmark>> getBookmarks(const MessageDisplayBookmarks::BookmarkFilter& filter, const MessageDisplayBookmarks::BookmarkOrder& order) const;

	std::vector<std::string> getActiveTokenNames() const;
	std::vector<std::string> getActiveTokenDisplayNames() const;
	std::vector<BookmarkCategory> getAllBookmarkCategories() const;
	bool activeTokenExists() const;
	std::shared_ptr<Bookmark> getBookmarkForActiveToken() const; // or null if no bookmark for that token exists

private:
	struct TempEdge
	{
	public:
		Id edgeId;
		NameHierarchy source;
		NameHierarchy target;
		int type;
	};

	virtual void handleMessage(MessageActivateBookmark* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageCreateBookmark* message);
	virtual void handleMessage(MessageCreateBookmarkCategory* message);
	virtual void handleMessage(MessageDeleteBookmark* message);
	virtual void handleMessage(MessageDeleteBookmarkCategoryWithBookmarks* message);
	virtual void handleMessage(MessageDeleteBookmarkForActiveTokens* message);
	virtual void handleMessage(MessageEditBookmark* message);
	virtual void handleMessage(MessageFinishedParsing* message);

	std::vector<std::string> getTokenNames(const std::vector<Id>& ids) const;
	std::vector<std::string> getTokenDisplayNames(const std::vector<Id>& ids) const;
	std::vector<int> getTokenTypes(const std::vector<Id>& ids) const;
	std::string getDisplayName(Id id) const;
	TempEdge getEdge(const std::string& tokenName, const int tokenType) const;

	std::pair<std::string, std::string> seperateEdgeToken(const std::string& token) const;

	std::vector<std::shared_ptr<Bookmark>> getFilteredBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkFilter& filter) const;
	std::vector<std::shared_ptr<Bookmark>> getOrderedBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const MessageDisplayBookmarks::BookmarkOrder& order) const;
	std::vector<std::shared_ptr<Bookmark>> getDateOrderedBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const;
	std::vector<std::shared_ptr<Bookmark>> getNameOrderedBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const;

	void cleanBookmarkCategories();

	static bool bookmarkDateCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b);
	static bool bookmarkNameCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b);

	static const std::string m_edgeSeperatorToken;

	StorageAccess* m_storageAccess;
	std::vector<Id> m_activeTokens;
	std::vector<std::string> m_activeTokenNames;
	std::vector<std::string> m_activeTokenDisplayNames;
	std::vector<int> m_activeTokenTypes;

	std::vector<Id> m_activeEdges;
	std::vector<std::string> m_activeEdgeNames;
	std::vector<std::string> m_activeEdgeDisplayNames;
	std::vector<int> m_activeEdgeTypes;

	bool m_activeTokenExists;
};

#endif // BOOKMARK_CONTROLLER_H
