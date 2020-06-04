#include "catch.hpp"

#include "Graph.h"

namespace
{
class TestToken: public Token
{
public:
	TestToken(): Token(0) {}

	TestToken(const TestToken& other): Token(other) {}

	virtual bool isNode() const
	{
		return false;
	}

	virtual bool isEdge() const
	{
		return false;
	}

	void addComponent(std::shared_ptr<TokenComponent> component)
	{
		Token::addComponent(component);
	}

	template <typename ComponentType>
	std::shared_ptr<ComponentType> removeComponent()
	{
		return Token::removeComponent<ComponentType>();
	}

	virtual std::wstring getReadableTypeString() const
	{
		return L"";
	}
};

class TestComponent: public TokenComponent
{
public:
	virtual std::shared_ptr<TokenComponent> copy() const
	{
		return std::make_shared<TestComponent>(*this);
	}
};

class Test2Component: public TokenComponent
{
public:
	virtual std::shared_ptr<TokenComponent> copy() const
	{
		return std::make_shared<Test2Component>(*this);
	}
};
}	 // namespace

TEST_CASE("tokens save location ids")
{
	TestToken a;
	a.addLocationId(23);
	a.addLocationId(5);

	REQUIRE(a.getLocationIds().size() == 2);
	REQUIRE(a.getLocationIds()[0] == 23);
	REQUIRE(a.getLocationIds()[1] == 5);
}

TEST_CASE("tokens remove location ids")
{
	TestToken a;
	a.addLocationId(23);
	a.addLocationId(5);
	a.removeLocationId(42);
	a.removeLocationId(5);

	REQUIRE(a.getLocationIds().size() == 1);
	REQUIRE(a.getLocationIds()[0] == 23);
}

TEST_CASE("token saves component")
{
	TestToken a;
	std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
	a.addComponent(component);

	REQUIRE(a.getComponent<TestComponent>());
	REQUIRE(!a.getComponent<Test2Component>());

	REQUIRE(a.getComponent<TestComponent>() == component.get());
}

TEST_CASE("token saves multiple components")
{
	TestToken a;
	std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
	std::shared_ptr<Test2Component> component2 = std::make_shared<Test2Component>();
	a.addComponent(component2);
	a.addComponent(component);

	REQUIRE(a.getComponent<TestComponent>());
	REQUIRE(a.getComponent<Test2Component>());

	REQUIRE(a.getComponent<TestComponent>() == component.get());
	REQUIRE(a.getComponent<Test2Component>() == component2.get());
}

TEST_CASE("token removes component")
{
	TestToken a;
	std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
	std::shared_ptr<Test2Component> component2 = std::make_shared<Test2Component>();
	a.addComponent(component2);
	a.addComponent(component);
	std::shared_ptr<TestComponent> component3 = a.removeComponent<TestComponent>();

	REQUIRE(!a.getComponent<TestComponent>());
	REQUIRE(a.getComponent<Test2Component>());

	REQUIRE(component3.get() == component.get());
	REQUIRE(a.getComponent<Test2Component>() == component2.get());
}

TEST_CASE("token copies components when token is copied")
{
	TestToken a;
	std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
	std::shared_ptr<Test2Component> component2 = std::make_shared<Test2Component>();
	a.addComponent(component2);
	a.addComponent(component);

	TestToken b(a);

	REQUIRE(b.getComponent<TestComponent>());
	REQUIRE(b.getComponent<Test2Component>());

	REQUIRE(b.getComponent<TestComponent>() != component.get());
	REQUIRE(b.getComponent<Test2Component>() != component2.get());
}

TEST_CASE("nodes are nodes")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);

	REQUIRE(a.isNode());
	REQUIRE(!a.isEdge());
}

TEST_CASE("edges are edges")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(3, Edge::EDGE_USAGE, &a, &b);

	REQUIRE(!e.isNode());
	REQUIRE(e.isEdge());
}

TEST_CASE("set type of node from constructor")
{
	Node n(1, NodeType(NODE_FUNCTION), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	REQUIRE(NodeType(NODE_FUNCTION) == n.getType());
}

TEST_CASE("set type of node from non indexed")
{
	Node n(2, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	n.setType(NodeType(NODE_CLASS));
	REQUIRE(NodeType(NODE_CLASS) == n.getType());
}

TEST_CASE("can not change type of node after it was set")
{
	Node n(3, NodeType(NODE_NAMESPACE), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	n.setType(NodeType(NODE_CLASS));
	REQUIRE(NodeType(NODE_CLASS) != n.getType());
}

TEST_CASE("node can be copied and keeps same id")
{
	Node n(4, NodeType(NODE_NAMESPACE), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node n2(n);

	REQUIRE(&n != &n2);
	REQUIRE(n.getId() == n2.getId());
	REQUIRE(n.getName() == n2.getName());
	REQUIRE(n.getType() == n2.getType());
}

TEST_CASE("node type bit masking")
{
	Node n(1, NodeType(NODE_NAMESPACE), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	REQUIRE(n.isType(NODE_FUNCTION | NODE_NAMESPACE | NODE_CLASS));
	REQUIRE(!n.isType(NODE_FUNCTION | NODE_METHOD | NODE_CLASS));
}

TEST_CASE("get type of edges")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(3, Edge::EDGE_USAGE, &a, &b);

	REQUIRE(Edge::EDGE_USAGE == e.getType());
}

TEST_CASE("edge can be copied and keeps same id")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(3, Edge::EDGE_USAGE, &a, &b);
	Edge e2(e, &a, &b);

	REQUIRE(&e != &e2);
	REQUIRE(e.getId() == e2.getId());
	REQUIRE(e.getType() == e2.getType());
}

TEST_CASE("edge type bit masking")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(3, Edge::EDGE_USAGE, &a, &b);

	REQUIRE(e.isType(Edge::EDGE_MEMBER | Edge::EDGE_CALL | Edge::EDGE_USAGE));
	REQUIRE(!e.isType(Edge::EDGE_MEMBER | Edge::EDGE_CALL));
}

TEST_CASE("node finds child node")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node c(3, NodeType(NODE_SYMBOL), NameHierarchy(L"C", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(4, Edge::EDGE_MEMBER, &a, &b);
	Edge e2(5, Edge::EDGE_MEMBER, &a, &c);

	Node* x = a.findChildNode([](Node* n) { return n->getName() == L"C"; });

	REQUIRE(x == &c);
	REQUIRE(x != &b);
}

TEST_CASE("node can not find child node")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node c(3, NodeType(NODE_SYMBOL), NameHierarchy(L"C", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(4, Edge::EDGE_MEMBER, &a, &b);
	Edge e2(5, Edge::EDGE_MEMBER, &a, &c);

	Node* x = a.findChildNode([](Node* n) { return n->getName() == L"D"; });

	REQUIRE(!x);
}

TEST_CASE("node visits child nodes")
{
	Node a(1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node b(2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node c(3, NodeType(NODE_SYMBOL), NameHierarchy(L"C", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Edge e(4, Edge::EDGE_MEMBER, &a, &b);
	Edge e2(5, Edge::EDGE_MEMBER, &a, &c);

	std::vector<Node*> children;
	a.forEachChildNode([&children](Node* n) { return children.push_back(n); });

	REQUIRE(children.size() == 2);
	REQUIRE(children[0] == &b);
	REQUIRE(children[1] == &c);
}

TEST_CASE("graph saves nodes")
{
	Graph graph;
	Node* a = graph.createNode(
		1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node* b = graph.createNode(
		2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);

	REQUIRE(2 == graph.getNodeCount());
	REQUIRE(0 == graph.getEdgeCount());

	REQUIRE(graph.getNodeById(a->getId()));
	REQUIRE(L"A" == graph.getNodeById(a->getId())->getName());

	REQUIRE(graph.getNodeById(b->getId()));
	REQUIRE(L"B" == graph.getNodeById(b->getId())->getName());

	REQUIRE(!graph.getNodeById(0));
}

TEST_CASE("graph saves edges")
{
	Graph graph;

	Node* a = graph.createNode(
		1, NodeType(NODE_FUNCTION), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	Node* b = graph.createNode(
		2, NodeType(NODE_FUNCTION), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);

	Edge* e = graph.createEdge(3, Edge::EDGE_CALL, a, b);

	REQUIRE(2 == graph.getNodeCount());
	REQUIRE(1 == graph.getEdgeCount());

	REQUIRE(graph.getEdgeById(e->getId()));
	REQUIRE(Edge::EDGE_CALL == graph.getEdgeById(e->getId())->getType());
}

TEST_CASE("graph removes nodes")
{
	Graph graph;

	Node* a = graph.createNode(
		1, NodeType(NODE_SYMBOL), NameHierarchy(L"A", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);
	graph.createNode(
		2, NodeType(NODE_SYMBOL), NameHierarchy(L"B", NAME_DELIMITER_CXX), DEFINITION_EXPLICIT);

	REQUIRE(2 == graph.getNodeCount());
	REQUIRE(0 == graph.getEdgeCount());

	graph.removeNode(graph.getNodeById(a->getId()));

	REQUIRE(1 == graph.getNodeCount());
}
