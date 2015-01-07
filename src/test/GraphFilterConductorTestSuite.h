#include "cxxtest/TestSuite.h"

#include "data/graph/filter/GraphFilterConductor.h"
#include "data/query/QueryTree.h"
#include "TestStorage.h"

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

	void test_token_query_with_id()
	{
		std::set<Id> ids = getIdsForNodeWithName("main");
		std::stringstream ss;
		ss << "\"main";
		for (Id id : ids)
		{
			ss << ',' << id;
		}
		ss << '"';

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(ss.str()), // "main,<id>"

			"1 nodes: function:main\n"
			"0 edges:\n"
		);
	}

	void test_token_query_with_id_and_wrong_name_uses_id()
	{
		std::set<Id> ids = getIdsForNodeWithName("main");
		std::stringstream ss;
		ss << "\"hello";
		for (Id id : ids)
		{
			ss << ',' << id;
		}
		ss << '"';

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(ss.str()), // "hello,<id>"

			"1 nodes: function:main\n"
			"0 edges:\n"
		);
	}

	void test_token_query_with_ids()
	{
		std::set<Id> ids = getIdsForNodeWithName("A::A");
		std::stringstream ss;
		ss << "\"A::A";
		for (Id id : ids)
		{
			ss << ',' << id;
		}
		ss << '"';

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph(ss.str()), // "A::A,<id1>,<id2>"

			"2 nodes: method:A::A method:A::A\n"
			"0 edges:\n"
		);
	}

	void test_command_query()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("'method'"),

			"5 nodes: method:A::A method:A::A method:A::getCount method:A::process method:B::process\n"
			"0 edges:\n"
		);

		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("'class'"),

			"2 nodes: class:A class:B\n"
			"0 edges:\n"
		);
	}

	void test_operator_not()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("!'method'"),

			"7 nodes: "
				"class:A field:A::count undefined_type:int undefined_type:void class:B function:main "
				"undefined_function:B::B\n"
			"13 edges: "
				"child:A->A::count aggregation:A->int aggregation:A->void type_use:A::count->int inheritance:B->A "
				"aggregation:B->void aggregation:B->int return_type:main->int type_usage:main->B child:B->B::B "
				"call:main->B::B aggregation:main->B aggregation:main->A\n"
		);
	}

	void test_operator_sub()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("'class'.'base'"),

			"1 nodes: class:A\n"
			"0 edges:\n"
		);
	}

	void test_operator_has()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("\"A\">'field'"),

			"1 nodes: field:A::count\n"
			"0 edges:\n"
		);
	}

	void test_operator_or()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("('static'|'const')"),

			"4 nodes: field:A::count method:A::getCount method:A::process method:B::process\n"
			"0 edges:\n"
		);
	}

	void test_operator_group()
	{
		TS_ASSERT_EQUALS(
			printedFilteredTestGraph("('static'|'const').'public'"),

			"1 nodes: method:A::getCount\n"
			"0 edges:\n"
		);
	}

private:
	std::string printedFilteredTestGraph(std::string query)
	{
		QueryTree tree(query);
		GraphFilterConductor conductor;

		createTestStorage();
		Graph result;

		conductor.filter(&tree, &m_storage->getGraph(), &result);

		std::stringstream ss;
		result.printBasic(ss);
		return ss.str();
	}

	std::set<Id> getIdsForNodeWithName(const std::string& name)
	{
		createTestStorage();

		std::vector<SearchMatch> matches = m_storage->getAutocompletionMatches("", name);
		if (matches.size() && matches[0].fullName == name)
		{
			return matches[0].tokenIds;
		}

		return std::set<Id>();
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
			"	A(int c) {\n"
			"		count += c;\n"
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
