#include "TabsController.h"

#include "Application.h"
#include "MessageFind.h"
#include "MessageIndexingFinished.h"
#include "MessageScrollToLine.h"
#include "MessageSearch.h"
#include "MessageWindowChanged.h"
#include "ScreenSearchInterfaces.h"
#include "TabId.h"
#include "TaskLambda.h"
#include "TaskManager.h"
#include "TaskScheduler.h"

TabsController::TabsController(
	ViewLayout* mainLayout,
	const ViewFactory* viewFactory,
	StorageAccess* storageAccess,
	ScreenSearchSender* screenSearchSender)
	: m_mainLayout(mainLayout)
	, m_viewFactory(viewFactory)
	, m_storageAccess(storageAccess)
	, m_screenSearchSender(screenSearchSender)
	, m_isCreatingTab(false)
{
}

void TabsController::clear()
{
	getView()->clear();
	m_isCreatingTab = false;

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(m_tabsMutex);
			if (m_tabs.empty())
			{
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void TabsController::addTab(Id tabId, SearchMatch match)
{
	std::lock_guard<std::mutex> lock(m_tabsMutex);

	TaskManager::createScheduler(tabId)->startSchedulerLoopThreaded();

	m_tabs.emplace(
		tabId, std::make_shared<Tab>(tabId, m_viewFactory, m_storageAccess, m_screenSearchSender));

	MessageWindowChanged().dispatch();

	if (match.isValid())
	{
		MessageSearch msg({match}, NodeTypeSet::all());
		msg.setSchedulerId(tabId);
		msg.dispatch();

		if (match.tokenIds.size() && std::get<0>(m_scrollToLine) == match.tokenIds[0])
		{
			MessageScrollToLine scrollMsg(std::get<1>(m_scrollToLine), std::get<2>(m_scrollToLine));
			scrollMsg.setSchedulerId(tabId);
			scrollMsg.dispatch();
		}
	}
	else
	{
		MessageFind msg;
		msg.setSchedulerId(tabId);
		msg.dispatch();
	}

	m_scrollToLine = std::make_tuple(0, FilePath(), 0);
	m_isCreatingTab = false;
}

void TabsController::showTab(Id tabId)
{
	std::lock_guard<std::mutex> lock(m_tabsMutex);

	auto it = m_tabs.find(tabId);
	if (it != m_tabs.end())
	{
		TabId::setCurrentTabId(tabId);
		it->second->setParentLayout(m_mainLayout);
	}
	else
	{
		TabId::setCurrentTabId(0);
		m_mainLayout->showOriginalViews();
	}

	Task::dispatch(TabId::app(), std::make_shared<TaskLambda>([this]() {
					   m_screenSearchSender->clearMatches();
				   }));
}

void TabsController::removeTab(Id tabId)
{
	// use app task scheduler thread to stop running tasks of tab
	Task::dispatch(TabId::background(), std::make_shared<TaskLambda>([tabId, this]() {
					   m_screenSearchSender->clearMatches();

					   TaskScheduler* scheduler = TaskManager::getScheduler(tabId).get();
					   scheduler->terminateRunningTasks();
					   scheduler->stopSchedulerLoop();

					   TaskManager::destroyScheduler(tabId);

					   getView()->destroyTab(tabId);
				   }));
}

void TabsController::destroyTab(Id tabId)
{
	std::lock_guard<std::mutex> lock(m_tabsMutex);

	// destroy the tab on the qt thread to allow view destruction
	m_tabs.erase(tabId);

	if (m_tabs.empty() && Application::getInstance()->isProjectLoaded() && !m_isCreatingTab)
	{
		MessageTabOpen().dispatch();
		m_isCreatingTab = true;
	}
}

void TabsController::onClearTabs()
{
	TabId::setCurrentTabId(0);
	m_mainLayout->showOriginalViews();
}

TabsView* TabsController::getView() const
{
	return Controller::getView<TabsView>();
}

void TabsController::handleMessage(MessageActivateErrors* message)
{
	if (m_tabs.empty() && Application::getInstance()->isProjectLoaded())
	{
		MessageTabOpenWith(SearchMatch::createCommand(SearchMatch::COMMAND_ERROR)).dispatch();
	}
}

void TabsController::handleMessage(MessageIndexingFinished* message)
{
	if (m_tabs.empty() && Application::getInstance()->isProjectLoaded())
	{
		MessageTabOpenWith(SearchMatch::createCommand(SearchMatch::COMMAND_ALL)).dispatch();
	}
}

void TabsController::handleMessage(MessageTabClose* message)
{
	getView()->closeTab();
}

void TabsController::handleMessage(MessageTabOpen* message)
{
	if (Application::getInstance()->isProjectLoaded())
	{
		getView()->openTab(true, SearchMatch());
		m_isCreatingTab = true;
	}
}

void TabsController::handleMessage(MessageTabOpenWith* message)
{
	if (!Application::getInstance()->isProjectLoaded())
	{
		return;
	}

	SearchMatch match = message->match;
	if (!match.isValid())
	{
		Id tokenId = message->tokenId;
		if (!tokenId && message->locationId)
		{
			std::vector<Id> tokenIds = m_storageAccess->getNodeIdsForLocationIds(
				{message->locationId});
			if (tokenIds.size())
			{
				tokenId = tokenIds[0];
			}
		}

		if (!tokenId && !message->filePath.empty())
		{
			tokenId = m_storageAccess->getNodeIdForFileNode(message->filePath);

			if (message->line)
			{
				m_scrollToLine = std::make_tuple(tokenId, message->filePath, message->line);
			}
		}

		if (tokenId)
		{
			std::vector<SearchMatch> matches = m_storageAccess->getSearchMatchesForTokenIds({tokenId});
			if (matches.size())
			{
				match = matches[0];
			}
		}
	}

	if (match.isValid())
	{
		getView()->openTab(message->showTab, match);
		m_isCreatingTab = true;
	}
}

void TabsController::handleMessage(MessageTabSelect* message)
{
	getView()->selectTab(message->next);
}

void TabsController::handleMessage(MessageTabState* message)
{
	getView()->updateTab(message->tabId, message->searchMatches);
}
