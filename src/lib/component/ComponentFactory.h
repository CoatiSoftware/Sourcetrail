#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>

#include "data/access/LocationAccess.h"
#include "data/access/GraphAccess.h"

class Component;
class ViewFactory;
class ViewLayout;

class ComponentFactory
{
public:
	static std::shared_ptr<ComponentFactory> create(
		ViewFactory* viewFactory, GraphAccess* graphAccess, LocationAccess* locationAccess
	);

	~ComponentFactory();

	ViewFactory* getViewFactory() const;

	std::shared_ptr<Component> createCodeComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createGraphComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createRefreshComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createSearchComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createStatusBarComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createUndoRedoComponent(ViewLayout* viewLayout);

private:
	ComponentFactory();
	ComponentFactory(const ComponentFactory&);

	ViewFactory* m_viewFactory;

	GraphAccess* m_graphAccess;
	LocationAccess* m_locationAccess;
};

#endif // COMPONENT_FACTORY_H
