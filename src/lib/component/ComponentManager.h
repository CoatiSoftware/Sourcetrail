#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <vector>

#include "component/Component.h"
#include "component/ComponentFactory.h"
#include "data/access/LocationAccess.h"
#include "data/access/GraphAccess.h"

class View;
class ViewFactory;
class ViewLayout;

class ComponentManager
{
public:
	static std::shared_ptr<ComponentManager> create(
		ViewFactory* viewFactory, GraphAccess* graphAccess, LocationAccess* locationAccess
	);

	~ComponentManager();

	void setup(ViewLayout* viewLayout);

private:
	ComponentManager();
	ComponentManager(const ComponentManager&);

	std::shared_ptr<ComponentFactory> m_componentFactory;

	std::vector<std::shared_ptr<Component>> m_components;
	std::vector<std::shared_ptr<View>> m_views;
};

#endif // COMPONENT_MANAGER_H
