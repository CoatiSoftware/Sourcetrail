#include "BookmarkButtonsView.h"

BookmarkButtonsView::BookmarkButtonsView(ViewLayout* viewLayout): View(viewLayout), m_schedulerId(0)
{
}

std::string BookmarkButtonsView::getName() const
{
	return "BookmarkButtons";
}

Id BookmarkButtonsView::getSchedulerId() const
{
	return m_schedulerId;
}

void BookmarkButtonsView::setTabId(Id schedulerId)
{
	m_schedulerId = schedulerId;
}

void BookmarkButtonsView::handleMessage(MessageBookmarkButtonState* message)
{
	setCreateButtonState(message->state);
}
