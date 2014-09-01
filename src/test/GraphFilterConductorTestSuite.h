#include "cxxtest/TestSuite.h"

#include "data/graph/filter/GraphFilterConductor.h"
#include "data/query/QueryTree.h"
#include "utilityTest.h"

class GraphFilterConductorTestSuite : public CxxTest::TestSuite
{
public:
	void test_token_query()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("\"main\""),

			"1 nodes: function:main\n"
			"0 edges:\n"
		);
	}

	void test_command_query()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("method"),

			"4 nodes: method:A method:getCount method:process method:process\n"
			"0 edges:\n"
		);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("class"),

			"2 nodes: class:A class:B\n"
			"0 edges:\n"
		);
	}

	void test_operator_not()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("!method"),

			"7 nodes: "
				"class:A field:A::count undefined:int undefined:void class:B function:main undefined_function:B::B\n"
			"7 edges: "
				"child:A->A::count type_use:A::count->int inheritance:B->A return_type:main->int type_usage:main->B "
				"child:B->B::B call:main->B::B\n"
		);
	}

	void test_operator_sub()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("class.base"),

			"1 nodes: class:A\n"
			"0 edges:\n"
		);
	}

	void test_operator_has()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("\"A\":field"),

			"1 nodes: field:count\n"
			"0 edges:\n"
		);
	}

	void test_operator_or()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("(static|const)"),

			"4 nodes: field:count method:getCount method:process method:process\n"
			"0 edges:\n"
		);
	}

	void test_operator_group()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("(static|const).public"),

			"1 nodes: method:getCount\n"
			"0 edges:\n"
		);
	}

private:
	std::string printedFilteredTestGraph(std::string query)
	{
		QueryTree tree(query);
		GraphFilterConductor conductor;

		createTestGraph();
		Graph result;

		conductor.filter(&tree, &m_graph, &result);

		std::stringstream ss;
		result.printBasic(ss);
		return ss.str();
	}

	void createTestGraph()
	{
		if (m_graph.getNodeCount())
		{
			return;
		}

		m_graph = utility::getGraphForCxxCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	A() {\n"
			"		count++;\n"
			"	}\n"
			"\n"
			"	static int getCount()\n"
			"	{\n"
			"		return count;\n"
			"	}\n"
			"\n"
			"protected:\n"
			"	virtual void process() const = 0;\n"
			"\n"
			"private:\n"
			"	static int count;\n"
			"};\n"
			"\n"
			"class B\n"
			"	: public A\n"
			"{\n"
			"protected:\n"
			"	virtual void process() const\n"
			"	{\n"
			"		int number = 42;\n"
			"	}\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	B b;\n"
			"\n"
			"	return A::getCount();\n"
			"}\n"
		);
	}

	Graph m_graph;
};
