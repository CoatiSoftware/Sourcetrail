#include "component/ComponentManager.h"

#include "component/view/CodeView.h"
#include "component/view/GraphView.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	ViewFactory* viewFactory, ViewLayout* viewLayout, GraphAccess* graphAccess, LocationAccess* locationAccess
){
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(viewFactory, viewLayout, graphAccess, locationAccess);

	return ptr;
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::setup()
{
	std::shared_ptr<Component> graphComponent = m_componentFactory->createGraphComponent();
	m_components.push_back(graphComponent);

	std::shared_ptr<Component> codeComponent = m_componentFactory->createCodeComponent();
	m_components.push_back(codeComponent);

	std::shared_ptr<Component> undoRedoComponent = m_componentFactory->createUndoRedoComponent();
	m_components.push_back(undoRedoComponent);

	std::shared_ptr<Component> refreshComponent = m_componentFactory->createRefreshComponent();
	m_components.push_back(refreshComponent);

	std::shared_ptr<Component> searchComponent = m_componentFactory->createSearchComponent();
	m_components.push_back(searchComponent);

	std::shared_ptr<Component> statusBarComponent = m_componentFactory->createStatusBarComponent();
	m_components.push_back(statusBarComponent);
}

ComponentManager::ComponentManager()
{
}
