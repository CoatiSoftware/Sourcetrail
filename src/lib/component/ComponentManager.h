#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <vector>

#include "component/Component.h"
#include "component/ComponentFactory.h"

class CompositeView;
class StorageAccess;
class View;
class ViewFactory;
class ViewLayout;

class ComponentManager
{
public:
	static std::shared_ptr<ComponentManager> create(ViewFactory* viewFactory, StorageAccess* graphAccess);

	~ComponentManager();

	void setup(ViewLayout* viewLayout);

	void refreshViews();

private:
	ComponentManager();
	ComponentManager(const ComponentManager&);

	std::shared_ptr<ComponentFactory> m_componentFactory;

	std::vector<std::shared_ptr<CompositeView>> m_compositeViews;
	std::vector<std::shared_ptr<Component>> m_components;
};

#endif // COMPONENT_MANAGER_H
