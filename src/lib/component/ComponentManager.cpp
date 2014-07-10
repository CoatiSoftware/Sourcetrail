#include "component/ComponentManager.h"

#include "component/view/CodeView.h"
#include "component/view/GraphView.h"
#include "component/view/ViewLayout.h"

#include "data/location/TokenLocationFile.h"

std::shared_ptr<ComponentManager> ComponentManager::create(
	GuiFactory* guiFactory, ViewLayout* viewLayout, GraphAccess* graphAccess, LocationAccess* locationAccess
){
	std::shared_ptr<ComponentManager> ptr(new ComponentManager());
	ptr->m_componentFactory = ComponentFactory::create(guiFactory, viewLayout, graphAccess, locationAccess);

	return ptr;
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::setup()
{
	std::shared_ptr<Component> graphComponent = m_componentFactory->createGraphComponent();

	GraphView* graphView = graphComponent->getView<GraphView>();
	/*std::weak_ptr<GraphNode> node0 = graphView->addNode(Vec2i(-50, -50), "foo");
	graphView->addEdge(node0, graphView->addNode(Vec2i(100, 100), "bar"));
	graphView->addEdge(node0, graphView->addNode(Vec2i(50, -50), "war production co-ordinating commitee"));*/

	m_components.push_back(graphComponent);

	std::shared_ptr<Component> codeComponent = m_componentFactory->createCodeComponent();
	m_components.push_back(codeComponent);

	std::shared_ptr<Component> searchComponent = m_componentFactory->createSearchComponent();
	m_components.push_back(searchComponent);
}

ComponentManager::ComponentManager()
{
}
