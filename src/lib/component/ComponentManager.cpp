#include "component/ComponentManager.h"

#include "component/view/CodeView.h"
#include "component/view/GraphView.h"
#include "component/view/ViewLayout.h"

#include "data/location/TokenLocationFile.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	GuiFactory* guiFactory,
	ViewLayout* viewLayout,
	std::shared_ptr<LocationAccess> locationAccess,
	std::shared_ptr<GraphAccess> graphAccess)
{
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(guiFactory, viewLayout, locationAccess, graphAccess);

	return ptr;
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::setup()
{
	std::shared_ptr<Component> graphComponent = m_componentFactory->createGraphComponent();
	GraphView* graphView = graphComponent->getView<GraphView>();
	graphView->addNode(Vec2i(-100, -100), "foo");
	graphView->addNode(Vec2i(50, 50), "bar");

	m_components.push_back(graphComponent);

	std::shared_ptr<Component> codeComponent = m_componentFactory->createCodeComponent();
	m_components.push_back(codeComponent);

	CodeView* codeView = codeComponent->getView<CodeView>();

	std::string code =
		"class HelloWorld;\n"
		"static int n = 42; // the answer.\n"
		"\n"
		"int sum(int a, int b)\n"
		"{\n"
		"	return a + b;\n"
		"}\n";

	TokenLocationFile locationFile("test.cpp");
	locationFile.addTokenLocation(1, 1, 7, 1, 16);
	locationFile.addTokenLocation(2, 2, 8, 2, 10);
	locationFile.addTokenLocation(3, 2, 12, 2, 12);
	locationFile.addTokenLocation(4, 4, 1, 7, 1);

	codeView->addCodeSnippet(code, locationFile, 1);

	std::string code2 =
		"const char* name = new char[10];\n"
		"\n"
		"name = \"MetaVizz\\0\";";

	TokenLocationFile locationFile2("test.cpp");
	locationFile2.addTokenLocation(5, 123, 1, 123, 11);
	locationFile2.addTokenLocation(6, 123, 13, 123, 16);
	locationFile2.addTokenLocation(7, 125, 1, 125, 4);

	codeView->addCodeSnippet(code2, locationFile2, 123);

	// codeView->clearCodeSnippets();
}

ComponentManager::ComponentManager()
{
}
