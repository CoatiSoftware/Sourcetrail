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
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT_EQUALS(a, graph.getNode("A"));
		TS_ASSERT_EQUALS("A", graph.getNode("A")->getName());

		TS_ASSERT_EQUALS(b, graph.getNode("B"));
		TS_ASSERT_EQUALS("B", graph.getNode("B")->getName());

		TS_ASSERT(!graph.getNode("C"));
	}

	void test_graph_saves_edges()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		TS_ASSERT_EQUALS(e, graph.getEdge(Edge::EDGE_TYPE_OF, a, b));
		TS_ASSERT(!graph.getEdge(Edge::EDGE_CALL, a, b));
	}

	void test_graph_finds_nodes_and_edges_by_id()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		TS_ASSERT(!graph.getEdgeById(a->getId()));
		TS_ASSERT_EQUALS(a, graph.getNodeById(a->getId()));
		TS_ASSERT_EQUALS(a, graph.getTokenById(a->getId()));

		TS_ASSERT(!graph.getNodeById(e->getId()));
		TS_ASSERT_EQUALS(e, graph.getEdgeById(e->getId()));
		TS_ASSERT_EQUALS(e, graph.getTokenById(e->getId()));
	}

	void test_graph_creates_child_edges()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* ab = graph.createNodeHierarchy("A::B");

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(1, graph.getEdgeCount());

		TS_ASSERT(ab->getMemberEdge());
		TS_ASSERT(graph.getEdge(Edge::EDGE_MEMBER, a, ab));
		TS_ASSERT_EQUALS(ab->getMemberEdge(), graph.getEdge(Edge::EDGE_MEMBER, a, ab));
		TS_ASSERT_EQUALS(ab->getMemberEdge()->getFrom(), a);
		TS_ASSERT_EQUALS(ab->getMemberEdge()->getTo(), ab);
		TS_ASSERT_EQUALS(ab->getParentNode(), a);
	}

	void test_graph_removes_nodes()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");
		graph.createNodeHierarchy("A::C");
		graph.createNodeHierarchy("A::C::D");
		graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		graph.removeNode(a);

		TS_ASSERT_EQUALS(1, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT(!graph.getNode("A"));
		TS_ASSERT(!graph.getNode("A::C"));
		TS_ASSERT(graph.getNode("B"));
	}

	void test_graph_removes_edge()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		graph.removeEdge(e);

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT(graph.getNode("A"));
		TS_ASSERT(graph.getNode("B"));
	}

	void test_graph_can_not_remove_member_edge()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");
		Node* c = graph.createNodeHierarchy("A::C");
		graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		graph.removeEdge(c->getMemberEdge());

		TS_ASSERT_EQUALS(3, graph.getNodeCount());
		TS_ASSERT_EQUALS(2, graph.getEdgeCount());

		TS_ASSERT(graph.getNode("A"));
		TS_ASSERT(graph.getNode("B"));
		TS_ASSERT(graph.getNode("A::C"));
	}

	void test_node_in_graph_finds_child_node()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("A::B");
		Node* c = graph.createNodeHierarchy("A::C");

		Node* x = a->findChildNode(
			[](Node* n)
			{
				return n->getName() == "C";
			}
		);

		TS_ASSERT_EQUALS(x, c);
		TS_ASSERT_DIFFERS(x, b);
	}

	void test_node_has_name_and_full_name()
	{
		TestGraph graph;
		Node* n = graph.createNodeHierarchy("A::B::C");

		TS_ASSERT_EQUALS(n->getName(), "C");
		TS_ASSERT_EQUALS(n->getFullName(), "A::B::C");
	}

	void test_edge_has_name()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_FUNCTION, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_FUNCTION, "B");
		Edge* e = graph.createEdge(Edge::EDGE_CALL, a, b);

		TS_ASSERT_EQUALS(e->getName(), "call:A->B");
	}

	void test_graph_saves_nodes_with_distinct_signatures()
	{
		TestGraph graph;
		Node* a1 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, "A", "A1");
		Node* a2 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, "A", "A2");
		Node* a3 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, "A", "A2");

		TS_ASSERT_DIFFERS(a1, a2);
		TS_ASSERT_EQUALS(a2, a3);

		Node* c1 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, "B::C", "C1");
		Node* c2 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, "B::C", "C2");
		Node* c3 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, "B::C", "C2");

		TS_ASSERT_DIFFERS(c1, c2);
		TS_ASSERT_EQUALS(c2, c3);
	}

	void test_graph_saves_nodes_as_undefined_function_when_using_signatures()
	{
		TestGraph graph;
		Node* a1 = graph.createNodeHierarchyWithDistinctSignature("A", "A1");
		Node* a2 = graph.createNodeHierarchyWithDistinctSignature("A", "A2");

		TS_ASSERT_DIFFERS(a1, a2);
		TS_ASSERT_EQUALS(a1->getType(), Node::NODE_UNDEFINED_FUNCTION);
		TS_ASSERT_EQUALS(a2->getType(), Node::NODE_UNDEFINED_FUNCTION);
	}

	void test_graph_creates_multiple_nodes_as_undefined_nodes()
	{
		TestGraph graph;
		Node* abc = graph.createNodeHierarchy("A::B::C");

		TS_ASSERT_EQUALS(3, graph.getNodeCount());
		TS_ASSERT_EQUALS(2, graph.getEdgeCount());

		TS_ASSERT_EQUALS("A", graph.getNode("A")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A")->getType());

		TS_ASSERT_EQUALS("A::B", graph.getNode("A::B")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B")->getType());

		TS_ASSERT_EQUALS(abc, graph.getNode("A::B::C"));
		TS_ASSERT_EQUALS("A::B::C", graph.getNode("A::B::C")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C")->getType());

		Node* abcde = graph.createNodeHierarchy("A::B::C::D::E");

		TS_ASSERT_EQUALS(5, graph.getNodeCount());
		TS_ASSERT_EQUALS(4, graph.getEdgeCount());

		TS_ASSERT_EQUALS("A::B::C::D", graph.getNode("A::B::C::D")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C::D")->getType());

		TS_ASSERT_EQUALS(abcde, graph.getNode("A::B::C::D::E"));
		TS_ASSERT_EQUALS("A::B::C::D::E", graph.getNode("A::B::C::D::E")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C::D::E")->getType());
	}

	void test_visit_each_token_on_graph()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* b = graph.createNodeHierarchy("B");
		Node* c = graph.createNodeHierarchy("C");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, a, b);
		Edge* f = graph.createEdge(Edge::EDGE_TYPE_OF, b, c);

		unsigned long idSum = a->getId() + b->getId() + c->getId() + e->getId() + f->getId();
		unsigned long checkSum = 0;

		graph.forEachToken(
			[&checkSum](Token* t)
			{
				checkSum += t->getId();
			}
		);

		TS_ASSERT_EQUALS(idSum, checkSum);
	}

	void test_visit_each_edge_of_type_on_node()
	{
		TestGraph graph;
		Node* a = graph.createNodeHierarchy("A");
		Node* ab = graph.createNodeHierarchy("A::B");
		Node* ac = graph.createNodeHierarchy("A::C");

		graph.createEdge(Edge::EDGE_TYPE_OF, a, ab);
		graph.createEdge(Edge::EDGE_CALL, a, ac);

		unsigned int sum = 0;
		a->forEachEdgeOfType(Edge::EDGE_MEMBER, [&sum](Edge* e)
		{
			TS_ASSERT_EQUALS(Edge::EDGE_MEMBER, e->getType());
			sum++;
		});

		TS_ASSERT_EQUALS(sum, 2);
	}

	void test_creating_plain_copy_of_graph_part()
	{
		TestGraph graph;
		Node* b = graph.createNodeHierarchy("A::B");
		graph.createNodeHierarchy("A::B::C");
		Node* d = graph.createNodeHierarchy("D");
		Node* e = graph.createNodeHierarchy("E");
		graph.createEdge(Edge::EDGE_TYPE_OF, d, b);
		graph.createEdge(Edge::EDGE_TYPE_OF, d, e);

		TestGraph plainGraph;
		Node* x = graph.getNode("A::B");
		plainGraph.addNodeAsPlainCopy(x);

		x->forEachEdge(
			[&plainGraph](Edge* e)
			{
				plainGraph.addNodeAsPlainCopy(e->getFrom());
				plainGraph.addNodeAsPlainCopy(e->getTo());
				plainGraph.addEdgeAsPlainCopy(e);
			}
		);

		TS_ASSERT_EQUALS(4, plainGraph.getNodeCount());
		TS_ASSERT_EQUALS(3, plainGraph.getEdgeCount());

		TS_ASSERT(plainGraph.getNode("A"));
		TS_ASSERT(plainGraph.getNode("A::B"));
		TS_ASSERT(plainGraph.getNode("A::B::C"));
		TS_ASSERT(plainGraph.getNode("D"));
		TS_ASSERT(!plainGraph.getNode("E"));
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

	class TestGraph: public Graph
	{
	public:
		size_t getNodeCount() const
		{
			return getNodes().size();
		}

		size_t getEdgeCount() const
		{
			return getEdges().size();
		}
	};

};
