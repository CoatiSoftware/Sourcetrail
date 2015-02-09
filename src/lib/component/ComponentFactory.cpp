#include "component/ComponentFactory.h"

#include "component/Component.h"
#include "component/controller/CodeController.h"
#include "component/controller/GraphController.h"
#include "component/controller/RefreshController.h"
#include "component/controller/SearchController.h"
#include "component/controller/StatusBarController.h"
#include "component/controller/UndoRedoController.h"
#include "component/view/CodeView.h"
#include "component/view/GraphView.h"
#include "component/view/RefreshView.h"
#include "component/view/SearchView.h"
#include "component/view/StatusBarView.h"
#include "component/view/UndoRedoView.h"
#include "component/view/ViewFactory.h"

std::shared_ptr<ComponentFactory> ComponentFactory::create(
	ViewFactory* viewFactory, GraphAccess* graphAccess, LocationAccess* locationAccess
){
	std::shared_ptr<ComponentFactory> ptr(new ComponentFactory());

	ptr->m_viewFactory = viewFactory;
	ptr->m_graphAccess = graphAccess;
	ptr->m_locationAccess = locationAccess;

	return ptr;
}

ComponentFactory::~ComponentFactory()
{
}

ViewFactory* ComponentFactory::getViewFactory() const
{
	return m_viewFactory;
}

std::shared_ptr<Component> ComponentFactory::createCodeComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<CodeView> view = m_viewFactory->createCodeView(viewLayout);
	std::shared_ptr<CodeController> controller = std::make_shared<CodeController>(m_graphAccess, m_locationAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createGraphComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<View> view = m_viewFactory->createGraphView(viewLayout);
	std::shared_ptr<GraphController> controller = std::make_shared<GraphController>(m_graphAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createRefreshComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<View> view = m_viewFactory->createRefreshView(viewLayout);
	std::shared_ptr<RefreshController> controller = std::make_shared<RefreshController>();

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createSearchComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<SearchView> view = m_viewFactory->createSearchView(viewLayout);
	std::shared_ptr<SearchController> controller = std::make_shared<SearchController>(m_graphAccess);

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createUndoRedoComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<UndoRedoView> view = m_viewFactory->createUndoRedoView(viewLayout);
	std::shared_ptr<UndoRedoController> controller = std::make_shared<UndoRedoController>();

	return std::make_shared<Component>(view, controller);
}

std::shared_ptr<Component> ComponentFactory::createStatusBarComponent(ViewLayout* viewLayout)
{
	std::shared_ptr<StatusBarView> view = m_viewFactory->createStatusBarView(viewLayout);
	std::shared_ptr<StatusBarController> controller = std::make_shared<StatusBarController>();

	return std::make_shared<Component>(view, controller);
}

ComponentFactory::ComponentFactory()
{
}
