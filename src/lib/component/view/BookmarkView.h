#ifndef BOOKMARK_VIEW_H
#define BOOKMARK_VIEW_H

#include "component/view/View.h"
#include "data/bookmark/Bookmark.h"

class BookmarkController;

class BookmarkView
	: public View
{
public:
	BookmarkView(ViewLayout* viewLayout);
	virtual ~BookmarkView();

	virtual std::string getName() const;

	enum CreateButtonState
	{
		CAN_CREATE = 0,
		CANNOT_CREATE,
		ALREADY_CREATED
	};

	virtual void setCreateButtonState(const CreateButtonState& state) = 0;

	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks) = 0;
	virtual void displayBookmarkEditor(
		std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories) = 0;
	virtual void displayBookmarkCreator(
		const std::vector<std::wstring>& names, const std::vector<BookmarkCategory>& categories, Id nodeId) = 0;
	virtual void enableDisplayBookmarks(bool enable) = 0;

	virtual bool bookmarkBrowserIsVisible() const = 0;

private:
	BookmarkController* getController();
};

#endif // BOOKMARK_VIEW_H
