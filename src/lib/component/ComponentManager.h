#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <memory>
#include <vector>

#include "Component.h"
#include "ComponentFactory.h"
#include "DialogView.h"

class CompositeView;
class DialogView;
class ScreenSearchSender;
class StorageAccess;
class TabbedView;
class View;
class ViewFactory;
class ViewLayout;

class ComponentManager
{
public:
	ComponentManager(const ViewFactory* viewFactory, StorageAccess* storageAccess);

	void clear();

	void setupMain(ViewLayout* viewLayout, Id appId);
	void setupTab(ViewLayout* viewLayout, Id tabId, ScreenSearchSender* screenSearchSender);
	void teardownTab(ScreenSearchSender* screenSearchSender);

	void clearComponents();
	void refreshViews();

	View* getView(const std::string& name) const;
	std::shared_ptr<DialogView> getDialogView(DialogView::UseCase useCase) const;

private:
	ComponentFactory m_componentFactory;

	std::vector<std::shared_ptr<View>> m_singleViews;
	std::vector<std::shared_ptr<Component>> m_components;

	std::map<DialogView::UseCase, std::shared_ptr<DialogView>> m_dialogViews;
};

#endif	  // COMPONENT_MANAGER_H
