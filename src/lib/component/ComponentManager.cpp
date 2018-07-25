#include "component/ComponentManager.h"

#include "utility/logging/logging.h"

#include "component/controller/Controller.h"
#include "component/controller/ScreenSearchController.h"
#include "component/view/CompositeView.h"
#include "component/view/DialogView.h"
#include "component/view/TabbedView.h"
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

	std::shared_ptr<Component> bookmarkComponent = m_componentFactory->createBookmarkComponent(compositeView.get());
	m_components.push_back(bookmarkComponent);

	std::shared_ptr<Component> graphComponent = m_componentFactory->createGraphComponent(viewLayout);
	m_components.push_back(graphComponent);

	std::shared_ptr<Component> codeComponent = m_componentFactory->createCodeComponent(viewLayout);
	m_components.push_back(codeComponent);

	std::shared_ptr<Component> statusBarComponent = m_componentFactory->createStatusBarComponent(viewLayout);
	m_components.push_back(statusBarComponent);

	std::shared_ptr<Component> activationComponent = m_componentFactory->createActivationComponent();
	m_components.push_back(activationComponent);

	std::shared_ptr<Component> tooltipComponent = m_componentFactory->createTooltipComponent(viewLayout);
	m_components.push_back(tooltipComponent);

	std::shared_ptr<Component> screenSearchComponent = m_componentFactory->createScreenSearchComponent(viewLayout);
	ScreenSearchController* screenSearchController = screenSearchComponent->getController<ScreenSearchController>();
	screenSearchController->addResponder(dynamic_cast<ScreenSearchResponder*>(graphComponent->getViewPtr()));
	screenSearchController->addResponder(dynamic_cast<ScreenSearchResponder*>(codeComponent->getViewPtr()));
	m_components.push_back(screenSearchComponent);

	for (DialogView::UseCase useCase :
		{ DialogView::UseCase::GENERAL, DialogView::UseCase::INDEXING, DialogView::UseCase::PROJECT_SETUP })
	{
		m_dialogViews.emplace(
			useCase,
			m_componentFactory->getViewFactory()->createDialogView(viewLayout, useCase, m_componentFactory->getStorageAccess())
		);
	}

	m_dialogViews[DialogView::UseCase::INDEXING]->setDialogsHideable(true);

	std::shared_ptr<TabbedView> tabbedView =
		m_componentFactory->getViewFactory()->createTabbedView(viewLayout, "Status");
	m_tabbedViews.push_back(tabbedView);

	std::shared_ptr<Component> statusComponent = m_componentFactory->createStatusComponent(tabbedView.get());
	m_components.push_back(statusComponent);

	std::shared_ptr<Component> errorComponent = m_componentFactory->createErrorComponent(tabbedView.get());
	m_components.push_back(errorComponent);

}

void ComponentManager::clearComponents()
{
	for (const std::shared_ptr<Component>& component : m_components)
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
	for (const std::shared_ptr<Component>& component : m_components)
	{
		View* view = component->getView<View>();

		if (view)
		{
			view->refreshView();
		}
	}

	for (const std::shared_ptr<CompositeView>& view : m_compositeViews)
	{
		view->refreshView();
	}

	for (const std::shared_ptr<TabbedView>& view : m_tabbedViews)
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

ComponentManager::ComponentManager()
{
}
