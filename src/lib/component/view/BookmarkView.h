#ifndef BOOKMARK_VIEW_H
#define BOOKMARK_VIEW_H

#include "data/bookmark/Bookmark.h"

#include "component/view/View.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageDisplayBookmarkCreator.h"
#include "utility/messaging/type/MessageDisplayBookmarkEditor.h"

class BookmarkController;

class BookmarkView
	: public View
	, public MessageListener<MessageDisplayBookmarks>
	, public MessageListener<MessageDisplayBookmarkCreator>
	, public MessageListener<MessageDisplayBookmarkEditor>
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
	virtual void enableDisplayBookmarks(bool enable) = 0;

	virtual void update();

	virtual bool bookmarkBrowserIsVisible() const = 0;

private:
	BookmarkController* getController();

	virtual void handleMessage(MessageDisplayBookmarks* message);
	virtual void handleMessage(MessageDisplayBookmarkCreator* message);
	virtual void handleMessage(MessageDisplayBookmarkEditor* message);

	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks) = 0;
	virtual void displayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories) = 0;
	virtual void displayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories) = 0;

	MessageDisplayBookmarks::BookmarkFilter m_filter;
	MessageDisplayBookmarks::BookmarkOrder m_order;
};

#endif // BOOKMARK_VIEW_H
