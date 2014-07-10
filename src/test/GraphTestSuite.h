#include "cxxtest/TestSuite.h"

#include "data/graph/Graph.h"
#include "data/graph/edgeComponent/EdgeComponentDataType.h"
#include "data/type/modifier/DataTypeModifierPointer.h"

class GraphTestSuite : public CxxTest::TestSuite
{
public:
	void test_tokens_get_unique_id()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Node c(Node::NODE_UNDEFINED, "C");

		TS_ASSERT_DIFFERS(a.getId(), c.getId());
		TS_ASSERT_DIFFERS(a.getId(), b.getId());
		TS_ASSERT_DIFFERS(b.getId(), c.getId());
	}

	void test_get_and_set_type_of_nodes()
	{
		Node n(Node::NODE_NAMESPACE, "A");
		TS_ASSERT_EQUALS(Node::NODE_NAMESPACE, n.getType());

		n.setType(Node::NODE_CLASS);
		TS_ASSERT_EQUALS(Node::NODE_CLASS, n.getType());
	}

	void test_get_type_of_edges()
	{
		Node a(Node::NODE_UNDEFINED, "A");
		Node b(Node::NODE_UNDEFINED, "B");
		Edge e(Edge::EDGE_TYPE_OF, &a, &b);

		TS_ASSERT_EQUALS(Edge::EDGE_TYPE_OF, e.getType());
	}

	void test_get_and_set_const_and_static_of_nodes()
	{
		Node n(Node::NODE_GLOBAL_VARIABLE, "A");
		TS_ASSERT(!n.isConst());
		TS_ASSERT(!n.isStatic());

		n.setConst(true);
		TS_ASSERT(n.isConst());
		TS_ASSERT(!n.isStatic());

		n.setStatic(true);
		TS_ASSERT(n.isConst());
		TS_ASSERT(n.isStatic());
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

		TS_ASSERT(graph.getEdge(Edge::EDGE_MEMBER, a, ab));
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

	void test_graph_creates_multiple_nodes_as_type_nodes()
	{
		TestGraph graph;
		Node* abc = graph.createNodeHierarchy("A::B::C");

		TS_ASSERT_EQUALS(3, graph.getNodeCount());
		TS_ASSERT_EQUALS(2, graph.getEdgeCount());

		TS_ASSERT_EQUALS("A", graph.getNode("A")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A")->getType());

		TS_ASSERT_EQUALS("A::B", graph.getNode("A::B")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B")->getType());

		TS_ASSERT_EQUALS(abc, graph.getNode("A::B::C"));
		TS_ASSERT_EQUALS("A::B::C", graph.getNode("A::B::C")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C")->getType());

		Node* abcde = graph.createNodeHierarchy("A::B::C::D::E");

		TS_ASSERT_EQUALS(5, graph.getNodeCount());
		TS_ASSERT_EQUALS(4, graph.getEdgeCount());

		TS_ASSERT_EQUALS("A::B::C::D", graph.getNode("A::B::C::D")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C::D")->getType());

		TS_ASSERT_EQUALS(abcde, graph.getNode("A::B::C::D::E"));
		TS_ASSERT_EQUALS("A::B::C::D::E", graph.getNode("A::B::C::D::E")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C::D::E")->getType());
	}

	void test_set_access_on_child_edge_via_node()
	{
		TestGraph graph;
		Node* b = graph.createNodeHierarchy("A::B");
		b->getMemberEdge()->setAccess(Edge::ACCESS_PRIVATE);
		TS_ASSERT_EQUALS(Edge::ACCESS_PRIVATE, b->getMemberEdge()->getAccess());
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
		b->getMemberEdge()->setAccess(Edge::ACCESS_PUBLIC);
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
		TS_ASSERT_EQUALS(Edge::ACCESS_PUBLIC, plainGraph.getNode("A::B")->getMemberEdge()->getAccess());
	}

private:
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
