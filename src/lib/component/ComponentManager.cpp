#include "component/ComponentManager.h"

#include "component/view/CodeView.h"
#include "component/view/ViewLayout.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	GuiFactory* guiFactory,
	ViewLayout* viewLayout,
	std::shared_ptr<CodeAccess> codeAccess,
	std::shared_ptr<GraphAccess> graphAccess)
{
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(guiFactory, viewLayout, codeAccess, graphAccess);
	return ptr;
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::setup()
{
	std::shared_ptr<Component> codeComponent = m_componentFactory->createCodeComponent();
	m_components.push_back(codeComponent);

	CodeView* codeView = dynamic_cast<CodeView*>(codeComponent->getView());
	codeView->addCodeSnippet("class HelloWorld;");
	codeView->addCodeSnippet("static int n = 42; // the answer.");
	// codeView->clearCodeSnippets();
}

ComponentManager::ComponentManager()
{
}
