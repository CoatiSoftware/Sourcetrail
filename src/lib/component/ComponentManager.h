#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <vector>

#include "component/Component.h"
#include "component/ComponentFactory.h"
#include "data/access/LocationAccess.h"
#include "data/access/GraphAccess.h"

class GuiFactory;
class ViewLayout;

class ComponentManager
{
public:
	static std::shared_ptr<ComponentManager> create(
		GuiFactory* guiFactory, ViewLayout* viewLayout, GraphAccess* graphAccess, LocationAccess* locationAccess
	);

	~ComponentManager();

	void setup();

private:
	ComponentManager();
	ComponentManager(const ComponentManager&);

	std::shared_ptr<ComponentFactory> m_componentFactory;

	std::vector<std::shared_ptr<Component> > m_components;
};

#endif // COMPONENT_MANAGER_H
