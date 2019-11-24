#include "ComponentManager.h"

#include "BookmarkButtonsView.h"
#include "BookmarkView.h"
#include "CodeView.h"
#include "CompositeView.h"
#include "Controller.h"
#include "DialogView.h"
#include "GraphView.h"
#include "RefreshView.h"
#include "ScreenSearchController.h"
#include "SearchView.h"
#include "TabbedView.h"
#include "UndoRedoView.h"
#include "ViewFactory.h"
#include "logging.h"

namespace
{
template <class Container>
void reverseErase(Container & container)
{
	while(!container.empty())
		container.pop_back();
}
}	 // namespace

ComponentManager::ComponentManager(const ViewFactory* viewFactory, StorageAccess* storageAccess)
	: m_componentFactory(viewFactory, storageAccess)
{
}

void ComponentManager::clear()
{
	m_dialogViews.clear();
	reverseErase(m_components);
	reverseErase(m_singleViews);
}

void ComponentManager::setupMain(ViewLayout* viewLayout, Id appId)
{
	std::shared_ptr<CompositeView> compositeView =
		m_componentFactory.getViewFactory()->createCompositeView(
			viewLayout, CompositeView::DIRECTION_HORIZONTAL, "Search");
	m_singleViews.push_back(compositeView);

	std::shared_ptr<UndoRedoView> undoRedoView =
		m_componentFactory.getViewFactory()->createUndoRedoView(compositeView.get());
	std::shared_ptr<RefreshView> refreshView =
		m_componentFactory.getViewFactory()->createRefreshView(compositeView.get());
	std::shared_ptr<SearchView> searchView = m_componentFactory.getViewFactory()->createSearchView(
		compositeView.get());

	std::shared_ptr<BookmarkButtonsView> bookmarkView =
		m_componentFactory.getViewFactory()->createBookmarkButtonsView(compositeView.get());
	bookmarkView->setTabId(appId);

	std::shared_ptr<GraphView> graphView = m_componentFactory.getViewFactory()->createGraphView(
		viewLayout);
	std::shared_ptr<CodeView> codeView = m_componentFactory.getViewFactory()->createCodeView(
		viewLayout);

	for (std::shared_ptr<View> view: std::vector<std::shared_ptr<View>>(
			 {undoRedoView, searchView, bookmarkView, graphView, codeView}))
	{
		view->setEnabled(false);
	}

	m_singleViews.push_back(undoRedoView);
	m_singleViews.push_back(refreshView);
	m_singleViews.push_back(searchView);
	m_singleViews.push_back(bookmarkView);
	m_singleViews.push_back(graphView);
	m_singleViews.push_back(codeView);

	std::shared_ptr<Component> screenSearchComponent =
		m_componentFactory.createScreenSearchComponent(viewLayout);
	ScreenSearchController* screenSearchController =
		screenSearchComponent->getController<ScreenSearchController>();
	screenSearchController->addResponder(graphView.get());
	screenSearchController->addResponder(codeView.get());
	m_components.push_back(screenSearchComponent);

	std::shared_ptr<Component> tabsComponent = m_componentFactory.createTabsComponent(
		viewLayout, screenSearchController);
	m_components.push_back(tabsComponent);

	std::shared_ptr<Component> tooltipComponent = m_componentFactory.createTooltipComponent(
		viewLayout);
	m_components.push_back(tooltipComponent);

	for (DialogView::UseCase useCase:
		 {DialogView::UseCase::GENERAL,
		  DialogView::UseCase::INDEXING,
		  DialogView::UseCase::PROJECT_SETUP})
	{
		m_dialogViews.emplace(
			useCase,
			m_componentFactory.getViewFactory()->createDialogView(
				viewLayout, useCase, m_componentFactory.getStorageAccess()));
	}

	m_dialogViews[DialogView::UseCase::INDEXING]->setDialogsHideable(true);

	std::shared_ptr<TabbedView> tabbedView = m_componentFactory.getViewFactory()->createTabbedView(
		viewLayout, "Status");
	m_singleViews.push_back(tabbedView);

	std::shared_ptr<Component> statusComponent = m_componentFactory.createStatusComponent(
		tabbedView.get());
	m_components.push_back(statusComponent);

	std::shared_ptr<Component> errorComponent = m_componentFactory.createErrorComponent(
		tabbedView.get());
	m_components.push_back(errorComponent);

	std::shared_ptr<Component> bookmarkComponent = m_componentFactory.createBookmarkComponent(
		compositeView.get());
	m_components.push_back(bookmarkComponent);

	std::shared_ptr<Component> activationComponent = m_componentFactory.createActivationComponent();
	m_components.push_back(activationComponent);

	std::shared_ptr<Component> statusBarComponent = m_componentFactory.createStatusBarComponent(
		viewLayout);
	m_components.push_back(statusBarComponent);

	std::shared_ptr<Component> customTrailComponent = m_componentFactory.createCustomTrailComponent(
		viewLayout);
	m_components.push_back(customTrailComponent);
}

void ComponentManager::setupTab(ViewLayout* viewLayout, Id tabId, ScreenSearchSender* screenSearchSender)
{
	std::shared_ptr<CompositeView> compositeView =
		m_componentFactory.getViewFactory()->createCompositeView(
			viewLayout, CompositeView::DIRECTION_HORIZONTAL, "Search");
	m_singleViews.push_back(compositeView);

	std::shared_ptr<Component> undoRedoComponent = m_componentFactory.createUndoRedoComponent(
		compositeView.get());
	undoRedoComponent->setTabId(tabId);
	m_components.push_back(undoRedoComponent);

	std::shared_ptr<Component> refreshComponent = m_componentFactory.createRefreshComponent(
		compositeView.get());
	refreshComponent->setTabId(tabId);
	m_components.push_back(refreshComponent);

	std::shared_ptr<Component> searchComponent = m_componentFactory.createSearchComponent(
		compositeView.get());
	searchComponent->setTabId(tabId);
	m_components.push_back(searchComponent);

	std::shared_ptr<BookmarkButtonsView> bookmarkView =
		m_componentFactory.getViewFactory()->createBookmarkButtonsView(compositeView.get());
	bookmarkView->setTabId(tabId);
	m_singleViews.push_back(bookmarkView);

	std::shared_ptr<Component> graphComponent = m_componentFactory.createGraphComponent(viewLayout);
	graphComponent->setTabId(tabId);
	m_components.push_back(graphComponent);

	std::shared_ptr<Component> codeComponent = m_componentFactory.createCodeComponent(viewLayout);
	codeComponent->setTabId(tabId);
	m_components.push_back(codeComponent);

	screenSearchSender->addResponder(graphComponent->getView<GraphView>());
	screenSearchSender->addResponder(codeComponent->getView<CodeView>());
}

void ComponentManager::teardownTab(ScreenSearchSender* screenSearchSender)
{
	for (const std::shared_ptr<Component>& component: m_components)
	{
		screenSearchSender->removeResponder(component->getView<GraphView>());
		screenSearchSender->removeResponder(component->getView<CodeView>());
	}
}

void ComponentManager::clearComponents()
{
	for (const std::shared_ptr<Component>& component: m_components)
	{
		Controller* controller = component->getController<Controller>();

		if (controller)
		{
			controller->clear();
		}
	}
}

void ComponentManager::refreshViews()
{
	for (const std::shared_ptr<Component>& component: m_components)
	{
		View* view = component->getView<View>();

		if (view)
		{
			view->refreshView();
		}
	}

	for (const std::shared_ptr<View>& view: m_singleViews)
	{
		view->refreshView();
	}
}

std::shared_ptr<DialogView> ComponentManager::getDialogView(DialogView::UseCase useCase) const
{
	auto it = m_dialogViews.find(useCase);
	if (it == m_dialogViews.end())
	{
		LOG_ERROR_STREAM(<< "No DialogView available for useCase " << int(useCase));
		return nullptr;
	}

	return it->second;
}
