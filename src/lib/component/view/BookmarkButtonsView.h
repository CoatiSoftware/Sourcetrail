#ifndef BOOKMARK_BUTTONS_VIEW_H
#define BOOKMARK_BUTTONS_VIEW_H

#include "MessageBookmarkButtonState.h"
#include "MessageListener.h"
#include "View.h"

class BookmarkButtonsView
	: public View
	, public MessageListener<MessageBookmarkButtonState>
{
public:
	BookmarkButtonsView(ViewLayout* viewLayout);
	virtual ~BookmarkButtonsView() = default;

	std::string getName() const override;

	Id getSchedulerId() const override;
	void setTabId(Id schedulerId);

	virtual void setCreateButtonState(const MessageBookmarkButtonState::ButtonState& state) = 0;

private:
	void handleMessage(MessageBookmarkButtonState* message) override;

	Id m_schedulerId;
};

#endif	  // BOOKMARK_BUTTONS_VIEW_H
