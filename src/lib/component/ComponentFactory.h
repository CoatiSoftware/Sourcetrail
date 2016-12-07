#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>

#include "data/access/StorageAccess.h"

class Component;
class ViewFactory;
class ViewLayout;

class ComponentFactory
{
public:
	static std::shared_ptr<ComponentFactory> create(ViewFactory* viewFactory, StorageAccess* storageAccess);

	~ComponentFactory();

	ViewFactory* getViewFactory() const;
	StorageAccess* getStorageAccess() const;

	std::shared_ptr<Component> createCodeComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createErrorComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createFeatureComponent();
	std::shared_ptr<Component> createGraphComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createLogComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createRefreshComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createSearchComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createStatusBarComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createStatusComponent(ViewLayout* viewLayout);
	std::shared_ptr<Component> createUndoRedoComponent(ViewLayout* viewLayout);

private:
	ComponentFactory();
	ComponentFactory(const ComponentFactory&);

	ViewFactory* m_viewFactory;
	StorageAccess* m_storageAccess;
};

#endif // COMPONENT_FACTORY_H
