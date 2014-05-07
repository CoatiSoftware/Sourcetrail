#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>

#include "component/Component.h"
#include "component/view/ViewManager.h"
#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"
#include "gui/GuiElementFactory.h"

class ComponentFactory
{
public:
	static std::shared_ptr<ComponentFactory> create(
		std::shared_ptr<ViewManager> viewManager,
		std::shared_ptr<GuiElementFactory> guiElementFactory,
		std::shared_ptr<CodeAccess> codeAccess,
		std::shared_ptr<GraphAccess> graphAccess);

	~ComponentFactory();

	std::shared_ptr<Component> createDummyComponent();

private:
	ComponentFactory();
	ComponentFactory(const ComponentFactory&);

	std::shared_ptr<ViewManager> m_viewManger;
	std::shared_ptr<GuiElementFactory> m_guiElementFactory;

	std::shared_ptr<CodeAccess> m_codeAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;
};


#endif // COMPONENT_FACTORY_H
