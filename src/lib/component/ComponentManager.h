#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <vector>

#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"
#include "component/Component.h"
#include "component/ComponentFactory.h"
#include "component/view/ViewManager.h"
#include "gui/GuiElementFactory.h"

class ComponentManager
{
public:
	static std::shared_ptr<ComponentManager> create(
		std::shared_ptr<ViewManager> viewManager,
		std::shared_ptr<GuiElementFactory> guiElementFactory,
		std::shared_ptr<CodeAccess> codeAccess,
		std::shared_ptr<GraphAccess> graphAccess);

	~ComponentManager();

	void setup();

private:
	ComponentManager();
	ComponentManager(const ComponentManager&);

	std::shared_ptr<ComponentFactory> m_componentFactory;

	std::vector<std::shared_ptr<Component> > m_components;
};


#endif // COMPONENT_MANAGER_H
