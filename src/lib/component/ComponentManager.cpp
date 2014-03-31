#include "component/ComponentManager.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	const std::shared_ptr<ViewManager>& viewManager,
	const std::shared_ptr<GuiElementFactory>& guiElementFactory,
	const std::shared_ptr<CodeAccess>& codeAccess,
	const std::shared_ptr<GraphAccess>& graphAccess)
{
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(viewManager, guiElementFactory, codeAccess, graphAccess);
	return ptr;
}

ComponentManager::ComponentManager()
{
}

ComponentManager::~ComponentManager()
{
}
