#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>

#include "data/access/LocationAccess.h"
#include "data/access/GraphAccess.h"

class Component;
class GuiFactory;
class ViewLayout;

class ComponentFactory
{
public:
	static std::shared_ptr<ComponentFactory> create(
		GuiFactory* guiFactory,
		ViewLayout* viewLayout,
		std::shared_ptr<LocationAccess> locationAccess,
		std::shared_ptr<GraphAccess> graphAccess);

	~ComponentFactory();

	std::shared_ptr<Component> createCodeComponent();
	std::shared_ptr<Component> createGraphComponent();

private:
	ComponentFactory();
	ComponentFactory(const ComponentFactory&);

	GuiFactory* m_guiFactory;
	ViewLayout* m_viewLayout;

	std::shared_ptr<LocationAccess> m_locationAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;
};


#endif // COMPONENT_FACTORY_H
