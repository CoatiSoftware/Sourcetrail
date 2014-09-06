#include "cxxtest/TestSuite.h"

#include "data/graph/filter/GraphFilter.h"
#include "data/graph/filter/GraphFilterImplementations.h"
#include "TestStorage.h"

class GraphFilterTestSuite : public CxxTest::TestSuite
{
public:
	void test_empty_GraphFilter()
	{
		GraphFilter filter;

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"0 nodes:\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandMember()
	{
		GraphFilterCommandMember filter;

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"6 nodes: "
				"method:A::A field:A::count method:A::getCount method:A::process method:B::process "
				"undefined_function:B::B\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandParent()
	{
		GraphFilterCommandParent filter;

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"2 nodes: class:A class:B\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandNodeType()
	{
		GraphFilterCommandNodeType filter(Node::NODE_FUNCTION | Node::NODE_METHOD);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"5 nodes: method:A::A method:A::getCount method:A::process method:B::process function:main\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandConst()
	{
		GraphFilterCommandConst filter;

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"2 nodes: method:A::process method:B::process\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandStatic()
	{
		GraphFilterCommandStatic filter;

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"2 nodes: field:A::count method:A::getCount\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandAccessType()
	{
		GraphFilterCommandAccessType filter(TokenComponentAccess::ACCESS_PROTECTED);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"2 nodes: method:A::process method:B::process\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandAbstractionType()
	{
		GraphFilterCommandAbstractionType filter(TokenComponentAbstraction::ABSTRACTION_PURE_VIRTUAL);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"1 nodes: method:A::process\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandCall()
	{
		GraphFilterCommandCall filter(true);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"1 nodes: function:main\n"
			"0 edges:\n"
		);

		GraphFilterCommandCall filter2(false);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter2),

			"2 nodes: method:A::getCount undefined_function:B::B\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandUsage()
	{
		GraphFilterCommandUsage filter;

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"6 nodes: method:A::A field:A::count method:A::getCount method:A::process method:B::process function:main\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterCommandInheritance()
	{
		GraphFilterCommandInheritance filter(true);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"1 nodes: class:A\n"
			"0 edges:\n"
		);

		GraphFilterCommandInheritance filter2(false);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter2),

			"1 nodes: class:B\n"
			"0 edges:\n"
		);
	}

	void test_GraphFilterToken()
	{
		GraphFilterToken filter("main");

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(&filter),

			"1 nodes: function:main\n"
			"0 edges:\n"
		);
	}

private:
	std::string printedFilteredTestGraph(GraphFilter* filter)
	{
		createTestStorage();
		Graph result;

		filter->apply(&m_storage->getGraph(), &result);

		std::stringstream ss;
		result.printBasic(ss);
		return ss.str();
	}

	void createTestStorage()
	{
		if (m_storage)
		{
			return;
		}

		m_storage = std::make_shared<TestStorage>();

		m_storage->parseCxxCode(
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

	std::shared_ptr<TestStorage> m_storage;
};
