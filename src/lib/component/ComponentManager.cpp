#include "component/ComponentManager.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	std::shared_ptr<ViewManager> viewManager,
	std::shared_ptr<GuiElementFactory> guiElementFactory,
	std::shared_ptr<CodeAccess> codeAccess,
	std::shared_ptr<GraphAccess> graphAccess)
{
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(viewManager, guiElementFactory, codeAccess, graphAccess);
	return ptr;
}

void ComponentManager::setup()
{
	m_components.push_back(m_componentFactory->createDummyComponent());
}

ComponentManager::ComponentManager()
{
}

ComponentManager::~ComponentManager()
{
}
