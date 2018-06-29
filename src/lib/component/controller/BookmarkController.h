#ifndef BOOKMARK_CONTROLLER_H
#define BOOKMARK_CONTROLLER_H

#include "data/bookmark/Bookmark.h"
#include "data/bookmark/NodeBookmark.h"
#include "data/bookmark/EdgeBookmark.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/error/MessageActivateErrors.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateBookmark.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageDisplayBookmarkCreator.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageFinishedParsing.h"

#include "component/controller/Controller.h"

class StorageAccess;

class BookmarkController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateBookmark>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageDisplayBookmarkCreator>
	, public MessageListener<MessageDisplayBookmarks>
	, public MessageListener<MessageFinishedParsing>
{
public:
	BookmarkController(StorageAccess* storageAccess);
	virtual ~BookmarkController();

	virtual void clear();

	void displayBookmarks();
	void displayBookmarksFor(Bookmark::BookmarkFilter filter, Bookmark::BookmarkOrder order);

	void createBookmark(const std::wstring& name, const std::wstring& comment, const std::wstring& category, Id nodeId);
	void editBookmark(Id bookmarkId, const std::wstring& name, const std::wstring& comment, const std::wstring& category);

	void deleteBookmark(Id bookmarkId);
	void deleteBookmarkCategory(Id categoryId);
	void deleteBookmarkForActiveTokens();

	void activateBookmark(const std::shared_ptr<Bookmark> bookmark);

	void showBookmarkCreator(Id nodeId = 0);
	void showBookmarkEditor(const std::shared_ptr<Bookmark> bookmark);

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

	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateBookmark* message);
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageDisplayBookmarkCreator* message);
	virtual void handleMessage(MessageDisplayBookmarks* message);
	virtual void handleMessage(MessageFinishedParsing* message);

	std::vector<std::wstring> getActiveTokenDisplayNames() const;
	std::vector<std::wstring> getDisplayNamesForNodeId(Id nodeId) const;

	std::vector<BookmarkCategory> getAllBookmarkCategories() const;

	std::shared_ptr<Bookmark> getBookmarkForActiveToken() const;
	std::shared_ptr<Bookmark> getBookmarkForNodeId(Id nodeId) const;

	bool canCreateBookmark() const;

	std::vector<std::shared_ptr<Bookmark>> getAllBookmarks() const;
	std::vector<std::shared_ptr<NodeBookmark>> getAllNodeBookmarks() const;
	std::vector<std::shared_ptr<EdgeBookmark>> getAllEdgeBookmarks() const;
	std::vector<std::shared_ptr<Bookmark>> getBookmarks(
		Bookmark::BookmarkFilter filter, Bookmark::BookmarkOrder order) const;

	std::vector<std::wstring> getActiveNodeDisplayNames() const;
	std::vector<std::wstring> getActiveEdgeDisplayNames() const;
	std::wstring getNodeDisplayName(const Id id) const;

	std::vector<std::shared_ptr<Bookmark>> getFilteredBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, Bookmark::BookmarkFilter filter) const;
	std::vector<std::shared_ptr<Bookmark>> getOrderedBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, Bookmark::BookmarkOrder order) const;
	std::vector<std::shared_ptr<Bookmark>> getDateOrderedBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const;
	std::vector<std::shared_ptr<Bookmark>> getNameOrderedBookmarks(
		const std::vector<std::shared_ptr<Bookmark>>& bookmarks, const bool ascending) const;

	void cleanBookmarkCategories();

	static bool bookmarkDateCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b);
	static bool bookmarkNameCompare(const std::shared_ptr<Bookmark> a, const std::shared_ptr<Bookmark> b);

	void update();

	static const std::wstring s_edgeSeperatorToken;
	static const std::wstring s_defaultCategoryName;

	StorageAccess* m_storageAccess;
	mutable BookmarkCache m_bookmarkCache;

	std::vector<Id> m_activeNodeIds;
	std::vector<Id> m_activeEdgeIds;

	Bookmark::BookmarkFilter m_filter;
	Bookmark::BookmarkOrder m_order;
};

#endif // BOOKMARK_CONTROLLER_H
