#include "cxxtest/TestSuite.h"

#include "data/graph/Graph.h"

class GraphTestSuite : public CxxTest::TestSuite
{
public:
	void test_tokens_get_unique_id()
	{
		TestToken a;
		TestToken b;
		TestToken c;

		TS_ASSERT_DIFFERS(a.getId(), c.getId());
		TS_ASSERT_DIFFERS(a.getId(), b.getId());
		TS_ASSERT_DIFFERS(b.getId(), c.getId());
	}

	void test_tokens_save_location_ids()
	{
		TestToken a;
		a.addLocationId(23);
		a.addLocationId(5);

		TS_ASSERT_EQUALS(a.getLocationIds().size(), 2);
		TS_ASSERT_EQUALS(a.getLocationIds()[0], 23);
		TS_ASSERT_EQUALS(a.getLocationIds()[1], 5);
	}

	void test_tokens_remove_location_ids()
	{
		TestToken a;
		a.addLocationId(23);
		a.addLocationId(5);
		a.removeLocationId(42);
		a.removeLocationId(5);

		TS_ASSERT_EQUALS(a.getLocationIds().size(), 1);
		TS_ASSERT_EQUALS(a.getLocationIds()[0], 23);
	}

	void test_token_saves_component()
	{
		TestToken a;
		std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
		a.addComponent(component);

		TS_ASSERT(a.getComponent<TestComponent>());
		TS_ASSERT(!a.getComponent<Test2Component>());

		TS_ASSERT_EQUALS(a.getComponent<TestComponent>(), component.get());
	}

	void test_token_saves_multiple_components()
	{
		TestToken a;
		std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
		std::shared_ptr<Test2Component> component2 = std::make_shared<Test2Component>();
		a.addComponent(component2);
		a.addComponent(component);

		TS_ASSERT(a.getComponent<TestComponent>());
		TS_ASSERT(a.getComponent<Test2Component>());

		TS_ASSERT_EQUALS(a.getComponent<TestComponent>(), component.get());
		TS_ASSERT_EQUALS(a.getComponent<Test2Component>(), component2.get());
	}

	void test_token_removes_component()
	{
		TestToken a;
		std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
		std::shared_ptr<Test2Component> component2 = std::make_shared<Test2Component>();
		a.addComponent(component2);
		a.addComponent(component);
		std::shared_ptr<TestComponent> component3 = a.removeComponent<TestComponent>();

		TS_ASSERT(!a.getComponent<TestComponent>());
		TS_ASSERT(a.getComponent<Test2Component>());

		TS_ASSERT_EQUALS(component3.get(), component.get());
		TS_ASSERT_EQUALS(a.getComponent<Test2Component>(), component2.get());
	}

	void test_token_copies_components_when_token_is_copied()
	{
		TestToken a;
		std::shared_ptr<TestComponent> component = std::make_shared<TestComponent>();
		std::shared_ptr<Test2Component> component2 = std::make_shared<Test2Component>();
		a.addComponent(component2);
		a.addComponent(component);

		TestToken b(a);

		TS_ASSERT(b.getComponent<TestComponent>());
		TS_ASSERT(b.getComponent<Test2Component>());

		TS_ASSERT_DIFFERS(b.getComponent<TestComponent>(), component.get());
		TS_ASSERT_DIFFERS(b.getComponent<Test2Component>(), component2.get());
	}

	void test_nodes_are_nodes()
	{
		Node a(Node::NODE_UNDEFINED, "A");

		TS_ASSERT(a.isNode());
		TS_ASSERT(!a.isEdge());
	}

	void test_edges_are_edges()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Edge e(Edge::EDGE_TYPE_OF, &a, &b);

		TS_ASSERT(!e.isNode());
		TS_ASSERT(e.isEdge());
	}

	void test_set_type_of_node_from_constructor()
	{
		Node n(Node::NODE_FUNCTION, "A");
		TS_ASSERT_EQUALS(Node::NODE_FUNCTION, n.getType());
	}

	void test_set_type_of_node_from_undefined()
	{
		Node n(Node::NODE_UNDEFINED, "A");
		n.setType(Node::NODE_CLASS);
		TS_ASSERT_EQUALS(Node::NODE_CLASS, n.getType());
	}

	void test_can_not_change_type_of_node_after_it_was_set()
	{
		Node n(Node::NODE_NAMESPACE, "A");
		n.setType(Node::NODE_CLASS);
		TS_ASSERT_DIFFERS(Node::NODE_CLASS, n.getType());
	}

	void test_node_can_be_copied_and_keeps_same_id()
	{
		Node n(Node::NODE_NAMESPACE, "A");
		Node n2(n);

		TS_ASSERT_DIFFERS(&n, &n2);
		TS_ASSERT_EQUALS(n.getId(), n2.getId());
		TS_ASSERT_EQUALS(n.getName(), n2.getName());
		TS_ASSERT_EQUALS(n.getType(), n2.getType());
	}

	void test_node_type_bit_masking()
	{
		Node n(Node::NODE_NAMESPACE, "A");
		TS_ASSERT(n.isType(Node::NODE_FUNCTION | Node::NODE_NAMESPACE | Node::NODE_CLASS));
		TS_ASSERT(!n.isType(Node::NODE_FUNCTION | Node::NODE_METHOD | Node::NODE_CLASS));
	}

	void test_get_type_of_edges()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Edge e(Edge::EDGE_TYPE_OF, &a, &b);

		TS_ASSERT_EQUALS(Edge::EDGE_TYPE_OF, e.getType());
	}

	void test_edge_can_be_copied_and_keeps_same_id()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Edge e(Edge::EDGE_TYPE_OF, &a, &b);
		Edge e2(e, &a, &b);

		TS_ASSERT_DIFFERS(&e, &e2);
		TS_ASSERT_EQUALS(e.getId(), e2.getId());
		TS_ASSERT_EQUALS(e.getType(), e2.getType());
	}

	void test_edge_type_bit_masking()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Edge e(Edge::EDGE_TYPE_OF, &a, &b);

		TS_ASSERT(e.isType(Edge::EDGE_MEMBER | Edge::EDGE_CALL | Edge::EDGE_TYPE_OF));
		TS_ASSERT(!e.isType(Edge::EDGE_USAGE | Edge::EDGE_MEMBER | Edge::EDGE_CALL));
	}

	void test_node_finds_child_node()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Node c(Node::NODE_UNDEFINED, "C");
		Edge e(Edge::EDGE_MEMBER, &a, &b);
		Edge e2(Edge::EDGE_MEMBER, &a, &c);

		Node* x = a.findChildNode(
			[](Node* n)
			{
				return n->getName() == "C";
			}
		);

		TS_ASSERT_EQUALS(x, &c);
		TS_ASSERT_DIFFERS(x, &b);
	}

	void test_node_can_not_find_child_node()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Node c(Node::NODE_UNDEFINED, "C");
		Edge e(Edge::EDGE_MEMBER, &a, &b);
		Edge e2(Edge::EDGE_MEMBER, &a, &c);

		Node* x = a.findChildNode(
			[](Node* n)
			{
				return n->getName() == "D";
			}
		);

		TS_ASSERT(!x);
	}

	void test_node_visits_child_nodes()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Node c(Node::NODE_UNDEFINED, "C");
		Edge e(Edge::EDGE_MEMBER, &a, &b);
		Edge e2(Edge::EDGE_MEMBER, &a, &c);

		std::vector<Node*> children;
		a.forEachChildNode(
			[&children](Node* n)
			{
				return children.push_back(n);
			}
		);

		TS_ASSERT_EQUALS(children.size(), 2);
		TS_ASSERT_EQUALS(children[0], &b);
		TS_ASSERT_EQUALS(children[1], &c);
	}

	void test_graph_saves_nodes()
	{
		Graph graph;
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");

		graph.addNode(&a);
		graph.addNode(&b);

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT(graph.getNodeById(a.getId()));
		TS_ASSERT_EQUALS("A", graph.getNodeById(a.getId())->getName());

		TS_ASSERT(graph.getNodeById(b.getId()));
		TS_ASSERT_EQUALS("B", graph.getNodeById(b.getId())->getName());

		TS_ASSERT(!graph.getNodeById(0));
	}

	void test_graph_saves_edges()
	{
		Graph graph;

		Node a(Node::NODE_FUNCTION, "A");
		Node b(Node::NODE_FUNCTION, "B");

		Edge e(Edge::EDGE_CALL, &a, &b);

		graph.addEdge(&e);

		TS_ASSERT_EQUALS(0, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT(!graph.getEdgeById(e.getId()));

		graph.addNode(&a);
		graph.addNode(&b);

		graph.addEdge(&e);

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(1, graph.getEdgeCount());

		TS_ASSERT(graph.getEdgeById(e.getId()));
		TS_ASSERT_EQUALS(Edge::EDGE_CALL, graph.getEdgeById(e.getId())->getType());
	}

private:
	class TestToken: public Token
	{
	public:
		TestToken()
		{
		}

		TestToken(const TestToken& other)
			: Token(other)
		{
		}

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

		template<typename ComponentType>
		std::shared_ptr<ComponentType> removeComponent()
		{
			return Token::removeComponent<ComponentType>();
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
};
