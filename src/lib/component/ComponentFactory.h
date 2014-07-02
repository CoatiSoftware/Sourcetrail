#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <memory>

#include "component/Component.h"
#include "data/access/CodeAccess.h"
#include "data/access/GraphAccess.h"

class GuiFactory;
class ViewLayout;

class ComponentFactory
{
public:
	static std::shared_ptr<ComponentFactory> create(
		GuiFactory* guiFactory,
		ViewLayout* viewLayout,
		std::shared_ptr<CodeAccess> codeAccess,
		std::shared_ptr<GraphAccess> graphAccess);

	~ComponentFactory();

	std::shared_ptr<Component> createCodeComponent();

private:
	ComponentFactory();
	ComponentFactory(const ComponentFactory&);

	GuiFactory* m_guiFactory;
	ViewLayout* m_viewLayout;

	std::shared_ptr<CodeAccess> m_codeAccess;
	std::shared_ptr<GraphAccess> m_graphAccess;
};


#endif // COMPONENT_FACTORY_H
