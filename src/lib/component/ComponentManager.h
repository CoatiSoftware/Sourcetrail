#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <vector>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageRefresh.h"

#include "component/Component.h"
#include "component/ComponentFactory.h"

class CompositeView;
class StorageAccess;
class View;
class ViewFactory;
class ViewLayout;

class ComponentManager
	: public MessageListener<MessageRefresh>
{
public:
	static std::shared_ptr<ComponentManager> create(ViewFactory* viewFactory, StorageAccess* graphAccess);

	~ComponentManager();

	void setup(ViewLayout* viewLayout);

private:
	ComponentManager();
	ComponentManager(const ComponentManager&);

	void handleMessage(MessageRefresh* message);

	std::shared_ptr<ComponentFactory> m_componentFactory;

	std::vector<std::shared_ptr<CompositeView>> m_compositeViews;
	std::vector<std::shared_ptr<Component>> m_components;
};

#endif // COMPONENT_MANAGER_H
