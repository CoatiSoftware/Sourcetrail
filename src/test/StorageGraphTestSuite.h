#include "cxxtest/TestSuite.h"

#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/StorageGraph.h"
#include "data/search/SearchIndex.h"

class StorageGraphTestSuite : public CxxTest::TestSuite
{
public:
	void test_graph_saves_nodes()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_CLASS, "B");

		TS_ASSERT(a);
		TS_ASSERT(b);

		TS_ASSERT_EQUALS("A", a->getName());
		TS_ASSERT_EQUALS("B", b->getName());

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
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_CLASS, "B");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		TS_ASSERT_EQUALS(e, graph.getEdge(Edge::EDGE_TYPE_OF, a, b));
		TS_ASSERT(!graph.getEdge(Edge::EDGE_CALL, a, b));
	}

	void test_graph_finds_nodes_and_edges_by_id()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_CLASS, "B");
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
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* ab = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B");

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(1, graph.getEdgeCount());

		TS_ASSERT(ab->getMemberEdge());
		TS_ASSERT(graph.getEdge(Edge::EDGE_MEMBER, a, ab));
		TS_ASSERT_EQUALS(ab->getMemberEdge(), graph.getEdge(Edge::EDGE_MEMBER, a, ab));
		TS_ASSERT_EQUALS(ab->getMemberEdge()->getFrom(), a);
		TS_ASSERT_EQUALS(ab->getMemberEdge()->getTo(), ab);
		TS_ASSERT_EQUALS(ab->getParentNode(), a);
	}

	void test_graph_creates_aggregation_edges()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* ab = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B");
		Node* c = graph.createNodeHierarchy(Node::NODE_CLASS, "C");
		Node* cd = graph.createNodeHierarchy(Node::NODE_CLASS, "C::D");
		Node* cdd = graph.createNodeHierarchy(Node::NODE_METHOD, "C::D::D");
		Edge* i = graph.createEdge(Edge::EDGE_INHERITANCE, ab, cd);
		Edge* t = graph.createEdge(Edge::EDGE_TYPE_USAGE, cdd, a);

		TS_ASSERT_EQUALS(5, graph.getNodeCount());
		TS_ASSERT_EQUALS(8, graph.getEdgeCount());

		Edge* e1 = graph.getEdge(Edge::EDGE_AGGREGATION, a, c);
		TS_ASSERT(e1);
		TS_ASSERT_EQUALS(2, e1->getComponent<TokenComponentAggregation>()->getAggregationCount());
		TS_ASSERT_EQUALS(i->getId(), *e1->getComponent<TokenComponentAggregation>()->getAggregationIds().begin());
		TS_ASSERT_EQUALS(t->getId(), *(++e1->getComponent<TokenComponentAggregation>()->getAggregationIds().begin()));

		Edge* e2 = graph.getEdge(Edge::EDGE_AGGREGATION, ab, c);
		TS_ASSERT(e2);
		TS_ASSERT_EQUALS(1, e2->getComponent<TokenComponentAggregation>()->getAggregationCount());
		TS_ASSERT_EQUALS(i->getId(), *e2->getComponent<TokenComponentAggregation>()->getAggregationIds().begin());
	}

	void test_graph_removes_nodes()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "B");
		graph.createNodeHierarchy(Node::NODE_CLASS, "A::C");
		graph.createNodeHierarchy(Node::NODE_CLASS, "A::C::D");
		graph.createEdge(Edge::EDGE_TYPE_OF, b, a);

		graph.removeNode(a);

		TS_ASSERT_EQUALS(1, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT(!graph.getNode("A"));
		TS_ASSERT(!graph.getNode("A::C"));
		TS_ASSERT(graph.getNode("B"));
	}

	void test_graph_removes_edge()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_CLASS, "B");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, a, b);

		graph.removeEdge(e);

		TS_ASSERT_EQUALS(2, graph.getNodeCount());
		TS_ASSERT_EQUALS(0, graph.getEdgeCount());

		TS_ASSERT(graph.getNode("A"));
		TS_ASSERT(graph.getNode("B"));
	}

	void test_graph_can_not_remove_member_edge()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "B");
		Node* c = graph.createNodeHierarchy(Node::NODE_CLASS, "A::C");
		graph.createEdge(Edge::EDGE_TYPE_OF, b, a);

		graph.removeEdge(c->getMemberEdge());

		TS_ASSERT_EQUALS(3, graph.getNodeCount());
		TS_ASSERT_EQUALS(2, graph.getEdgeCount());

		TS_ASSERT(graph.getNode("A"));
		TS_ASSERT(graph.getNode("B"));
		TS_ASSERT(graph.getNode("A::C"));
	}

	void test_node_in_graph_finds_child_node()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B");
		Node* c = graph.createNodeHierarchy(Node::NODE_CLASS, "A::C");

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
		TestStorageGraph graph;
		Node* n = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B::C");

		TS_ASSERT_EQUALS(n->getName(), "C");
		TS_ASSERT_EQUALS(n->getFullName(), "A::B::C");
	}

	void test_edge_has_name()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_FUNCTION, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_FUNCTION, "B");
		Edge* e = graph.createEdge(Edge::EDGE_CALL, a, b);

		TS_ASSERT_EQUALS(e->getName(), "call:A->B");
	}

	void test_graph_saves_nodes_with_distinct_signatures()
	{
		TestStorageGraph graph;
		Node* a1 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, "A", 1);
		Node* a2 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, "A", 2);
		Node* a3 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, "A", 2);

		TS_ASSERT_DIFFERS(a1, a2);
		TS_ASSERT_EQUALS(a2, a3);

		Node* c1 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, "B::C", 3);
		Node* c2 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, "B::C", 4);
		Node* c3 = graph.createNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, "B::C", 4);

		TS_ASSERT_DIFFERS(c1, c2);
		TS_ASSERT_EQUALS(c2, c3);
	}

	void test_graph_creates_multiple_nodes_as_undefined_nodes()
	{
		TestStorageGraph graph;
		Node* abc = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B::C");

		TS_ASSERT_EQUALS(3, graph.getNodeCount());
		TS_ASSERT_EQUALS(2, graph.getEdgeCount());

		TS_ASSERT_EQUALS("A", graph.getNode("A")->getName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A")->getType());

		TS_ASSERT_EQUALS("A::B", graph.getNode("A::B")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B")->getType());

		TS_ASSERT_EQUALS(abc, graph.getNode("A::B::C"));
		TS_ASSERT_EQUALS("A::B::C", graph.getNode("A::B::C")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_CLASS, graph.getNode("A::B::C")->getType());

		Node* abcde = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B::C::D::E");

		TS_ASSERT_EQUALS(5, graph.getNodeCount());
		TS_ASSERT_EQUALS(4, graph.getEdgeCount());

		TS_ASSERT_EQUALS("A::B::C::D", graph.getNode("A::B::C::D")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_UNDEFINED, graph.getNode("A::B::C::D")->getType());

		TS_ASSERT_EQUALS(abcde, graph.getNode("A::B::C::D::E"));
		TS_ASSERT_EQUALS("A::B::C::D::E", graph.getNode("A::B::C::D::E")->getFullName());
		TS_ASSERT_EQUALS(Node::NODE_CLASS, graph.getNode("A::B::C::D::E")->getType());
	}

	void test_visit_each_token_on_graph()
	{
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* b = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "B");
		Node* c = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "C");
		Edge* e = graph.createEdge(Edge::EDGE_TYPE_OF, b, a);
		Edge* f = graph.createEdge(Edge::EDGE_TYPE_OF, c, a);

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
		TestStorageGraph graph;
		Node* a = graph.createNodeHierarchy(Node::NODE_CLASS, "A");
		Node* ab = graph.createNodeHierarchy(Node::NODE_FIELD, "A::B");
		Node* ac = graph.createNodeHierarchy(Node::NODE_METHOD, "A::C");

		graph.createEdge(Edge::EDGE_TYPE_OF, ab, a);
		graph.createEdge(Edge::EDGE_USAGE, ac, ab);

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
		TestStorageGraph graph;
		Node* b = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B");
		Node* c = graph.createNodeHierarchy(Node::NODE_CLASS, "A::B::C");
		Node* d = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "D");
		Node* e = graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, "E");
		graph.createEdge(Edge::EDGE_TYPE_OF, d, b);
		graph.createEdge(Edge::EDGE_TYPE_OF, e, c);

		TestStorageGraph plainGraph;
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

		TS_ASSERT_EQUALS(5, plainGraph.getNodeCount());
		TS_ASSERT_EQUALS(4, plainGraph.getEdgeCount());

		TS_ASSERT(plainGraph.getNode("A"));
		TS_ASSERT(plainGraph.getNode("A::B"));
		TS_ASSERT(plainGraph.getNode("A::B::C"));
		TS_ASSERT(plainGraph.getNode("D"));
		TS_ASSERT(plainGraph.getNode("E"));
	}

private:
	class TestStorageGraph
		: public StorageGraph
	{
	public:
		Node* createNodeHierarchy(Node::NodeType type, const std::string& name)
		{
			SearchNode* searchNode = m_index.addNode(utility::splitToVector(name, "::"));
			return StorageGraph::createNodeHierarchy(type, searchNode);
		}

		Node* createNodeHierarchyWithDistinctSignature(
			Node::NodeType type, const std::string& name, Id signatureId
		){
			SearchNode* searchNode = m_index.addNode(utility::splitToVector(name, "::"));
			std::shared_ptr<TokenComponentSignature> signature = std::make_shared<TokenComponentSignature>(signatureId);
			return StorageGraph::createNodeHierarchyWithDistinctSignature(type, searchNode, signature);
		}

		Node* getNode(const std::string& fullName) const
		{
			return findNode(
				[&fullName](Node* node)
				{
					return node->getFullName() == fullName;
				}
			);
		}

		Edge* getEdge(Edge::EdgeType type, Node* from, Node* to) const
		{
			return from->findEdgeOfType(type,
				[to](Edge* edge)
				{
					return edge->getTo() == to;
				}
			);
		}

	private:
		SearchIndex m_index;
	};
};
