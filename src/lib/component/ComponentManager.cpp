#include "component/ComponentManager.h"

#include "component/view/CodeView.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	std::shared_ptr<ViewManager> viewManager,
	std::shared_ptr<GuiElementFactory> guiElementFactory,
	std::shared_ptr<CodeAccess> codeAccess,
	std::shared_ptr<GraphAccess> graphAccess)
{
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(viewManager, guiElementFactory, codeAccess, graphAccess);
	return ptr;
}

void ComponentManager::setup()
{
	m_components.push_back(m_componentFactory->createDummyComponent());

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

ComponentManager::~ComponentManager()
{
}
