#include "ComponentFactory.h"

#include "controller/ActivationController.h"
#include "controller/BookmarkController.h"
#include "view/BookmarkView.h"
#include "controller/CodeController.h"
#include "view/CodeView.h"
#include "Component.h"
#include "controller/CustomTrailController.h"
#include "view/CustomTrailView.h"
#include "controller/ErrorController.h"
#include "view/ErrorView.h"
#include "controller/GraphController.h"
#include "view/GraphView.h"
#include "controller/RefreshController.h"
#include "view/RefreshView.h"
#include "controller/ScreenSearchController.h"
#include "view/ScreenSearchView.h"
#include "controller/SearchController.h"
#include "view/SearchView.h"
#include "controller/StatusBarController.h"
#include "view/StatusBarView.h"
#include "controller/StatusController.h"
#include "view/StatusView.h"
#include "controller/TabsController.h"
#include "view/TabsView.h"
#include "controller/TooltipController.h"
#include "view/TooltipView.h"
#include "controller/UndoRedoController.h"
#include "view/UndoRedoView.h"
#include "view/ViewFactory.h"

ComponentFactory::ComponentFactory(const ViewFactory* viewFactory, StorageAccess* storageAccess)
	: m_viewFactory(viewFactory), m_storageAccess(storageAccess)
{
}

const ViewFactory* ComponentFactory::getViewFactory() const
{
	return m_viewFactory;
}

StorageAccess* ComponentFactory::getStorageAccess() const
{
	return m_storageAccess;
}

std::shared_ptr<Component> ComponentFactory::createActivationComponent()
{
	std::shared_ptr<Controller> controller = std::make_shared<ActivationController>(m_storageAccess);

	return std::make_shared<Component>(nullptr, controller);
}

std::shared_ptr<Component> ComponentFactory::createBookmarkComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<BookmarkView> view = m_viewFactory->createBookmarkView(viewLayout);
	std::shared_ptr<BookmarkController> controller = std::make_shared<BookmarkController>(
		m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createCodeComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<CodeView> view = m_viewFactory->createCodeView(viewLayout);
	std::shared_ptr<CodeController> controller = std::make_shared<CodeController>(m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createCustomTrailComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<CustomTrailView> view = m_viewFactory->createCustomTrailView(viewLayout);
	std::shared_ptr<CustomTrailController> controller = std::make_shared<CustomTrailController>(
		m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createErrorComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<ErrorView> view = m_viewFactory->createErrorView(viewLayout);
	std::shared_ptr<ErrorController> controller = std::make_shared<ErrorController>(m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createGraphComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<View> view = m_viewFactory->createGraphView(viewLayout);
	std::shared_ptr<GraphController> controller = std::make_shared<GraphController>(m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createRefreshComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<View> view = m_viewFactory->createRefreshView(viewLayout);
	std::shared_ptr<RefreshController> controller = std::make_shared<RefreshController>();

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createScreenSearchComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<ScreenSearchView> view = m_viewFactory->createScreenSearchView(viewLayout);
	std::shared_ptr<ScreenSearchController> controller = std::make_shared<ScreenSearchController>();

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createSearchComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<SearchView> view = m_viewFactory->createSearchView(viewLayout);
	std::shared_ptr<SearchController> controller = std::make_shared<SearchController>(m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createStatusBarComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<StatusBarView> view = m_viewFactory->createStatusBarView(viewLayout);
	std::shared_ptr<StatusBarController> controller = std::make_shared<StatusBarController>(
		m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createStatusComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<StatusView> view = m_viewFactory->createStatusView(viewLayout);
	std::shared_ptr<StatusController> controller = std::make_shared<StatusController>();

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createTabsComponent(
	ViewLayout* viewLayout, ScreenSearchSender* screenSearchSender)
{
	std::shared_ptr<TabsView> view = m_viewFactory->createTabsView(viewLayout);
	std::shared_ptr<Controller> controller = std::make_shared<TabsController>(
		viewLayout, m_viewFactory, m_storageAccess, screenSearchSender);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createTooltipComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<TooltipView> view = m_viewFactory->createTooltipView(viewLayout);
	std::shared_ptr<Controller> controller = std::make_shared<TooltipController>(m_storageAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createUndoRedoComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<UndoRedoView> view = m_viewFactory->createUndoRedoView(viewLayout);
	std::shared_ptr<UndoRedoController> controller = std::make_shared<UndoRedoController>(
		m_storageAccess);

	return std::make_shared<Component>(view, controller);
}
