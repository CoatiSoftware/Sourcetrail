#include "component/ComponentFactory.h"

#include "component/view/DummyView.h"
#include "component/controller/DummyController.h"

std::shared_ptr<ComponentFactory> ComponentFactory::create(
	std::shared_ptr<ViewManager> viewManager,
	std::shared_ptr<GuiElementFactory> guiElementFactory,
	std::shared_ptr<CodeAccess> codeAccess,
	std::shared_ptr<GraphAccess> graphAccess)
{
	std::shared_ptr<ComponentFactory> ptr(new ComponentFactory());
	ptr->m_viewManger = viewManager;
	ptr->m_guiElementFactory = guiElementFactory;
	ptr->m_codeAccess = codeAccess;
	ptr->m_graphAccess = graphAccess;
	return ptr;
}

ComponentFactory::~ComponentFactory()
{
}

std::shared_ptr<Component> ComponentFactory::createDummyComponent()
{
	std::shared_ptr<View> view = std::make_shared<DummyView>(m_viewManger, m_guiElementFactory);
	std::shared_ptr<Controller> controller = std::make_shared<DummyController>();

	std::shared_ptr<Component> component = std::make_shared<Component>(view, controller);
	return component;
}

ComponentFactory::ComponentFactory()
{
}
