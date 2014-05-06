#include "cxxtest/TestSuite.h"

#include "data/parser/cxx/CxxParser.h"
#include "data/parser/ParserClient.h"

class TestParserClient: public ParserClient
{
public:
	virtual void addClass(const ParseObject& object)
	{
		classes.push_back(object.name);
	}

	std::vector<std::string> classes;
};

class CxxParserTestSuite: public CxxTest::TestSuite
{
public:
	void test_cxx_parser_finds_classes()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();

		std::vector<std::string> filePaths;
		filePaths.push_back("data/test_code.cpp");

		CxxParser parser(client);
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->classes[0], "X::A");
		TS_ASSERT_EQUALS(client->classes[1], "B");
		TS_ASSERT_EQUALS(client->classes[2], "B::C");
	}
};
