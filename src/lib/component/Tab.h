#ifndef TAB_H
#define TAB_H

#include <vector>

#include "ComponentManager.h"
#include "../utility/messaging/type/focus/MessageFocusView.h"
#include "../utility/messaging/MessageListener.h"
#include "../utility/messaging/type/MessageRefreshUI.h"
#include "view/View.h"
#include "view/ViewLayout.h"

class Tab
	: public ViewLayout
	, public MessageListener<MessageFocusView>
	, public MessageListener<MessageRefreshUI>
{
public:
	Tab(Id tabId,
		const ViewFactory* viewFactory,
		StorageAccess* storageAccess,
		ScreenSearchSender* screenSearchSender);
	virtual ~Tab();

	Id getSchedulerId() const override;

	void setParentLayout(ViewLayout* parentLayout);

	// ViewLayout implementation
	void addView(View* view) override;
	void removeView(View* view) override;

	void showView(View* view) override;
	void hideView(View* view) override;

	void setViewEnabled(View* view, bool enabled) override;

private:
	void handleMessage(MessageFocusView* message) override;
	void handleMessage(MessageRefreshUI* message) override;

	const Id m_tabId;

	ComponentManager m_componentManager;
	std::vector<View*> m_views;

	ViewLayout* m_parentLayout;
	ScreenSearchSender* m_screenSearchSender;
};

#endif	  // TAB_H
