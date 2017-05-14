#include "component/view/BookmarkView.h"

#include "component/controller/BookmarkController.h"

BookmarkView::BookmarkView(ViewLayout* viewLayout)
	: View(viewLayout)
	, m_filter(MessageDisplayBookmarks::BookmarkFilter::ALL)
	, m_order(MessageDisplayBookmarks::BookmarkOrder::DATE_DESCENDING)
{
}

BookmarkView::~BookmarkView()
{
}

std::string BookmarkView::getName() const
{
	return "BookmarkView";
}

void BookmarkView::update()
{
	if (bookmarkBrowserIsVisible())
	{
		displayBookmarks(getController()->getBookmarks(m_filter, m_order));
	}
}

BookmarkController* BookmarkView::getController()
{
	return View::getController<BookmarkController>();
}

void BookmarkView::handleMessage(MessageDisplayBookmarks* message)
{
	if (bookmarkBrowserIsVisible() == true)
	{
		m_filter = message->filter;
		m_order = message->order;

		displayBookmarks(getController()->getBookmarks(message->filter, message->order));
	}

	displayBookmarks(getController()->getBookmarks(m_filter, m_order));
}

void BookmarkView::handleMessage(MessageDisplayBookmarkCreator* message)
{
	if (!getController()->canCreateBookmark() && !message->nodeId)
	{
		return;
	}

	if (message->nodeId)
	{
		if (getController()->getBookmarkForNodeId(message->nodeId) != nullptr)
		{
			displayBookmarkEditor(
				getController()->getBookmarkForNodeId(message->nodeId),
				getController()->getAllBookmarkCategories()
			);
		}
		else
		{
			displayBookmarkCreator(
				getController()->getDisplayNamesForNodeId(message->nodeId),
				getController()->getAllBookmarkCategories(),
				message->nodeId
			);
		}
	}
	else
	{
		if (getController()->getBookmarkForActiveToken() != nullptr)
		{
			displayBookmarkEditor(getController()->getBookmarkForActiveToken(), getController()->getAllBookmarkCategories());
		}
		else
		{
			displayBookmarkCreator(getController()->getActiveTokenDisplayNames(), getController()->getAllBookmarkCategories(), 0);
		}
	}
}

void BookmarkView::handleMessage(MessageDisplayBookmarkEditor* message)
{
	displayBookmarkEditor(message->bookmark, getController()->getAllBookmarkCategories());
}
