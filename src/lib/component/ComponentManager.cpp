#include "component/ComponentManager.h"

#include "component/controller/NetworkFactory.h"

#include "component/controller/Controller.h"
#include "component/view/CodeView.h"
#include "component/view/CompositeView.h"
#include "component/view/DialogView.h"
#include "component/view/GraphView.h"
#include "component/view/LogView.h"
#include "component/view/RefreshView.h"
#include "component/view/SearchView.h"
#include "component/view/TabbedView.h"
#include "component/view/UndoRedoView.h"
#include "component/view/ViewFactory.h"

std::shared_ptr<ComponentManager> ComponentManager::create(ViewFactory* viewFactory, StorageAccess* storageAccess)
{
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());

	ptr->m_componentFactory = ComponentFactory::create(viewFactory, storageAccess);

	return ptr;
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::setup(ViewLayout* viewLayout)
{
	std::shared_ptr<CompositeView> compositeView =
		m_componentFactory->getViewFactory()->createCompositeView(viewLayout, CompositeView::DIRECTION_HORIZONTAL, "Search");
	m_compositeViews.push_back(compositeView);

	std::shared_ptr<Component> undoRedoComponent = m_componentFactory->createUndoRedoComponent(compositeView.get());
	m_components.push_back(undoRedoComponent);

	std::shared_ptr<Component> refreshComponent = m_componentFactory->createRefreshComponent(compositeView.get());
	m_components.push_back(refreshComponent);

	std::shared_ptr<Component> searchComponent = m_componentFactory->createSearchComponent(compositeView.get());
	m_components.push_back(searchComponent);

	std::shared_ptr<Component> graphComponent = m_componentFactory->createGraphComponent(viewLayout);
	m_components.push_back(graphComponent);

	std::shared_ptr<Component> codeComponent = m_componentFactory->createCodeComponent(viewLayout);
	m_components.push_back(codeComponent);

	std::shared_ptr<Component> statusBarComponent = m_componentFactory->createStatusBarComponent(viewLayout);
	m_components.push_back(statusBarComponent);

	std::shared_ptr<Component> featureComponent = m_componentFactory->createFeatureComponent();
	m_components.push_back(featureComponent);

	m_dialogView = m_componentFactory->getViewFactory()->createDialogView(viewLayout, m_componentFactory->getStorageAccess());

	std::shared_ptr<TabbedView> tabbedView =
		m_componentFactory->getViewFactory()->createTabbedView(viewLayout, "Log");
	m_tabbedViews.push_back(tabbedView);

	// std::shared_ptr<Component> logComponent = m_componentFactory->createLogComponent(tabbedView.get());
	// m_components.push_back(logComponent);

	std::shared_ptr<Component> errorComponent = m_componentFactory->createErrorComponent(tabbedView.get());
	m_components.push_back(errorComponent);
}

void ComponentManager::clearComponents()
{
	for (std::shared_ptr<Component> component : m_components)
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
	for (std::shared_ptr<Component> component : m_components)
	{
		View* view = component->getView<View>();

		if (view)
		{
			view->refreshView();
		}
	}

	for (std::shared_ptr<CompositeView> view : m_compositeViews)
	{
		view->refreshView();
	}

	for (std::shared_ptr<TabbedView> view : m_tabbedViews)
	{
		view->refreshView();
	}
}

DialogView* ComponentManager::getDialogView() const
{
	return m_dialogView.get();
}

ComponentManager::ComponentManager()
{
}
