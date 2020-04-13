#ifndef TABS_CONTROLLER_H
#define TABS_CONTROLLER_H

#include "../../utility/messaging/type/activation/MessageActivateErrors.h"
#include "../../utility/messaging/type/indexing/MessageIndexingFinished.h"
#include "../../utility/messaging/MessageListener.h"
#include "../../utility/messaging/type/tab/MessageTabClose.h"
#include "../../utility/messaging/type/tab/MessageTabOpen.h"
#include "../../utility/messaging/type/tab/MessageTabOpenWith.h"
#include "../../utility/messaging/type/tab/MessageTabSelect.h"
#include "../../utility/messaging/type/tab/MessageTabState.h"

#include "Controller.h"
#include "../Tab.h"
#include "../view/TabsView.h"

struct SearchMatch;

class StorageAccess;
class ViewFactory;
class ViewLayout;

class TabsController
	: public Controller
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageTabClose>
	, public MessageListener<MessageTabOpen>
	, public MessageListener<MessageTabOpenWith>
	, public MessageListener<MessageTabSelect>
	, public MessageListener<MessageTabState>
{
public:
	TabsController(
		ViewLayout* mainLayout,
		const ViewFactory* viewFactory,
		StorageAccess* storageAccess,
		ScreenSearchSender* screenSearchSender);

	// Controller implementation
	virtual void clear();

	void addTab(Id tabId, SearchMatch match);
	void showTab(Id tabId);
	void removeTab(Id tabId);
	void destroyTab(Id tabId);
	void onClearTabs();

private:
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageIndexingFinished* message);
	virtual void handleMessage(MessageTabClose* message);
	virtual void handleMessage(MessageTabOpen* message);
	virtual void handleMessage(MessageTabOpenWith* message);
	virtual void handleMessage(MessageTabSelect* message);
	virtual void handleMessage(MessageTabState* message);

	TabsView* getView() const;

	ViewLayout* m_mainLayout;
	const ViewFactory* m_viewFactory;
	StorageAccess* m_storageAccess;
	ScreenSearchSender* m_screenSearchSender;

	std::map<Id, std::shared_ptr<Tab>> m_tabs;
	std::mutex m_tabsMutex;

	bool m_isCreatingTab;
	std::tuple<Id, FilePath, size_t> m_scrollToLine;
};

#endif	  // TABS_CONTROLLER_H
