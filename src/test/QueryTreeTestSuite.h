#include "cxxtest/TestSuite.h"

#include "data/query/QueryTree.h"

class QueryTreeTestSuite : public CxxTest::TestSuite
{
public:
	void test_empty_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree(""),

			" INVALID\n"
		);
	}

	void test_invalid_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree(" -"),

			"- INVALID\n"
			"\"-\"\n"
		);
	}

	void test_command_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("'class'"),

			"'class'\n"
			"'class'\n"
		);
	}

	void test_invalid_command_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("banana"),

			"banana INVALID\n"
			"\"banana\"\n"
		);
	}

	void test_token_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\""),

			"\"A\"\n"
			"\"A\"\n"
		);
	}

	void test_invalid_token_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A"),

			"\"A INVALID\n"
			"\"A\"\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("\"\""),

			"\"\" INVALID\n"
			"\"\"\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("A\""),

			"A \" INVALID\n"
			"	\"A\"\n"
			". IMPLICIT\n"
			"	\"\"\n"
		);
	}

	void test_token_query_with_id()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A,1\""),

			"\"A,1\"\n"
			"\"A,1\"\n"
		);
	}

	void test_token_query_with_ids()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A,1,2\""),

			"\"A,1,2\"\n"
			"\"A,1,2\"\n"
		);
	}

	void test_operator_not_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("!'field'"),

			"! 'field'\n"
			"!\n"
			"	'field'\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("!!'field'"),

			"! ! 'field'\n"
			"!\n"
			"	!\n"
			"		'field'\n"
		);
	}

	void test_invalid_operator_not_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("!"),

			"! INVALID\n"
			"! INVALID\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("'field'!"),

			"'field' ! INVALID\n"
			"	'field'\n"
			"! INVALID\n"
		);
	}

	void test_operator_sub_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\".\"B\""),

			"\"A\" . \"B\"\n"
			"	\"A\"\n"
			".\n"
			"	\"B\"\n"
		);
	}

	void test_invalid_operator_sub_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\"."),

			"\"A\" . INVALID\n"
			"	\"A\"\n"
			". INVALID\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("."),

			". INVALID\n"
			". INVALID\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree(".\"A\""),

			". \"A\" INVALID\n"
			". INVALID\n"
			"	\"A\"\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\"..\"B\""),

			"\"A\" . . \"B\" INVALID\n"
			"	\"A\"\n"
			".\n"
			"	. INVALID\n"
			"		\"B\"\n"
		);
	}

	void test_operator_has_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\">\"B\""),

			"\"A\" > \"B\"\n"
			"	\"A\"\n"
			">\n"
			"	\"B\"\n"
		);
	}

	void test_operator_and_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\"&\"B\""),

			"\"A\" & \"B\"\n"
			"	\"A\"\n"
			"&\n"
			"	\"B\"\n"
		);
	}

	void test_operator_or_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\"|\"B\""),

			"\"A\" | \"B\"\n"
			"	\"A\"\n"
			"|\n"
			"	\"B\"\n"
		);
	}

	void test_operator_group_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("(\"A\")"),

			"( \"A\" )\n"
			"(\"A\")\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("(\"A\"|\"B\")"),

			"( \"A\" | \"B\" )\n"
			"	\"A\"\n"
			"(|)\n"
			"	\"B\"\n"
		);
	}

	void test_invalid_operator_group_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("()"),

			"( ) INVALID\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("(\"A\""),

			"( \"A\" INVALID\n"
			"(\"A\") INVALID\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\")"),

			"\"A\" ) INVALID\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree(")("),

			") ( INVALID\n"
		);
	}

	void test_implicit_operator_sub_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("\"A\"(\"B\")"),

			"\"A\" ( \"B\" )\n"
			"	\"A\"\n"
			". IMPLICIT\n"
			"	(\"B\")\n"
		);
	}

	void test_operator_precedence_not_before_sub()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("!'method'.!'const'"),

			"! 'method' . ! 'const'\n"
			"	!\n"
			"		'method'\n"
			".\n"
			"	!\n"
			"		'const'\n"
		);
	}

	void test_operator_precedence_sub_before_has()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("'namespace'.'class'>'method'"),

			"'namespace' . 'class' > 'method'\n"
			"		'namespace'\n"
			"	.\n"
			"		'class'\n"
			">\n"
			"	'method'\n"
		);
	}

	void test_operator_precedence_has_before_or()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("'class'>'method'|'field'"),

			"'class' > 'method' | 'field'\n"
			"		'class'\n"
			"	>\n"
			"		'method'\n"
			"|\n"
			"	'field'\n"
		);
	}

	void test_operator_precedence_respects_groups()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("'namespace'.('class'>'method')"),

			"'namespace' . ( 'class' > 'method' )\n"
			"	'namespace'\n"
			".\n"
			"		'class'\n"
			"	(>)\n"
			"		'method'\n"
		);

		TS_ASSERT_EQUALS(
			printedQueryTree("'class'>('method'|'field')"),

			"'class' > ( 'method' | 'field' )\n"
			"	'class'\n"
			">\n"
			"		'method'\n"
			"	(|)\n"
			"		'field'\n"
		);
	}

	void test_spaces_get_stripped_out_of_query()
	{
		TS_ASSERT_EQUALS(
			printedQueryTree("  \"Field  \">('method' | 'field')   .'const' |  'public' "),

			"\"Field\" > ( 'method' | 'field' ) . 'const' | 'public'\n"
			"		\"Field\"\n"
			"	>\n"
			"				'method'\n"
			"			(|)\n"
			"				'field'\n"
			"		.\n"
			"			'const'\n"
			"|\n"
			"	'public'\n"
		);
	}

private:
	std::string printedQueryTree(std::string query) const
	{
		QueryTree tree(query);
		std::stringstream ss;
		tree.print(ss);
		return ss.str();
	}
};
