#include "component/ComponentFactory.h"

std::shared_ptr<ComponentFactory> ComponentFactory::create(
	const std::shared_ptr<ViewManager>& viewManager,
	const std::shared_ptr<GuiElementFactory>& guiElementFactory,
	const std::shared_ptr<CodeAccess>& codeAccess,
	const std::shared_ptr<GraphAccess>& graphAccess)
{
	std::shared_ptr<ComponentFactory> ptr(new ComponentFactory());
	ptr->m_viewManger = viewManager;
	ptr->m_guiElementFactory = guiElementFactory;
	ptr->m_codeAccess = codeAccess;
	ptr->m_graphAccess = graphAccess;
	return ptr;
}

ComponentFactory::ComponentFactory()
{
}

ComponentFactory::~ComponentFactory()
{
}
