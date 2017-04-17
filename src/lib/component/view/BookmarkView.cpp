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
	std::vector<std::string> names = getController()->getActiveTokenDisplayNames();

	if (getController()->hasBookmarkForActiveToken())
	{
		std::vector<BookmarkCategory> categories = getController()->getAllBookmarkCategories();

		displayBookmarkEditor(getController()->getBookmarkForActiveToken(), categories);
	}
	else
	{
		for (unsigned int i = 0; i < names.size(); i++)
		{
			std::string name = names[i];

			// skip the first letter as to not insert a leading space
			for (unsigned int i = 1; i < name.size(); i++)
			{
				if (std::isupper(name[i]))
				{
					name.insert(i, 1, ' ');
					i++;
				}
			}

			names[i] = name;
		}

		displayBookmarkCreator(names, getController()->getAllBookmarkCategories());
	}
}

void BookmarkView::handleMessage(MessageDisplayBookmarkEditor* message)
{
	std::vector<BookmarkCategory> categories = getController()->getAllBookmarkCategories();

	displayBookmarkEditor(message->bookmark, categories);
}
