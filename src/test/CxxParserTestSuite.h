#include "cxxtest/TestSuite.h"

#include "data/parser/cxx/CxxParser.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseVariable.h"
#include "utility/text/TextAccess.h"

class CxxParserTestSuite: public CxxTest::TestSuite
{
public:
	void test_cxx_parser_finds_classes()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "B");
		TS_ASSERT_EQUALS(client->classes[1], "public B::C");
	}

	void test_cxx_parser_finds_structs()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "X::A");
	}

	void test_cxx_parser_finds_globals()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->globals.size(), 2);
		TS_ASSERT_EQUALS(client->globals[0], "char * name");
		TS_ASSERT_EQUALS(client->globals[1], "H g");
	}

	void test_cxx_parser_finds_fields()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->fields.size(), 3);
		TS_ASSERT_EQUALS(client->fields[0], "private static const int B::C::amount");
		TS_ASSERT_EQUALS(client->fields[1], "private const int B::count");
		TS_ASSERT_EQUALS(client->fields[2], "private H B::h");
	}

	void test_cxx_parser_finds_functions()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->functions.size(), 2);
		TS_ASSERT_EQUALS(client->functions[0], "int ceil(float a)");
		TS_ASSERT_EQUALS(client->functions[1], "int (anonymous namespace)::sum(int a, int b)");
	}

	void test_cxx_parser_finds_methods()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->methods.size(), 5);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B()");
		TS_ASSERT_EQUALS(client->methods[1], "public virtual void B::~B()");
		TS_ASSERT_EQUALS(client->methods[2], "protected pure virtual void B::process()");
		TS_ASSERT_EQUALS(client->methods[3], "private int B::getCount() const");
		TS_ASSERT_EQUALS(client->methods[4], "public void B::B()");
	}

	void test_cxx_parser_finds_namespaces()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->namespaces.size(), 2);
		TS_ASSERT_EQUALS(client->namespaces[0], "X");
		TS_ASSERT_EQUALS(client->namespaces[1], "(anonymous)");
	}

	void test_cxx_parser_finds_enums()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "X::E");
	}

	void test_cxx_parser_finds_enum_fields()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths(1, "data/test_code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->enumFields.size(), 2);
		TS_ASSERT_EQUALS(client->enumFields[0], "X::E::P");
		TS_ASSERT_EQUALS(client->enumFields[1], "X::E::Q");
	}

	void test_cxx_parser_finds_typedef_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text = "typedef unsigned int uint;\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "unsigned int -> uint");
	}

	void test_cxx_parser_finds_typedef_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace test\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "unsigned int -> test::uint");
	}

	void test_cxx_parser_finds_typedef_that_uses_type_defined_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace test\n"
			"{\n"
			"	struct TestStruct{};\n"
			"}\n"
			"typedef test::TestStruct globalTestStruct;\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "test::TestStruct -> globalTestStruct");
	}

private:
	class TestParserClient: public ParserClient
	{
	public:
		virtual void onTypedefParsed(
			const ParseLocation& location, const std::string& fullName, const std::string& underlyingFullName,
			AccessType access
		)
		{
			typedefs.push_back(addAccessPrefix(underlyingFullName + " -> " + fullName, access));
		}

		virtual void onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
		{
			classes.push_back(addAccessPrefix(fullName, access));
		}

		virtual void onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
		{
			structs.push_back(addAccessPrefix(fullName, access));
		}

		virtual void onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
		{
			globals.push_back(variableStr(variable));
		}

		virtual void onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
		{
			fields.push_back(addAccessPrefix(variableStr(variable), access));
		}

		virtual void onFunctionParsed(
			const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
			const std::vector<ParseVariable>& parameters
		)
		{
			functions.push_back(returnTypeName + " " + fullName + parameterStr(parameters));
		}

		virtual void onMethodParsed(
			const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
			const std::vector<ParseVariable>& parameters, AccessType access, AbstractionType abstraction,
			bool isConst, bool isStatic
		)
		{
			std::string str = returnTypeName + " " + fullName + parameterStr(parameters);
			str = addStaticPrefix(addAbstractionPrefix(str, abstraction), isStatic);
			str = addConstPrefix(addAccessPrefix(str, access), isConst, false);
			methods.push_back(str);
		}

		virtual void onNamespaceParsed(const ParseLocation& location, const std::string& fullName)
		{
			namespaces.push_back(fullName);
		}

		virtual void onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
		{
			enums.push_back(addAccessPrefix(fullName, access));
		}

		virtual void onEnumFieldParsed(const ParseLocation& location, const std::string& fullName)
		{
			enumFields.push_back(fullName);
		}

		std::vector<std::string> typedefs;
		std::vector<std::string> classes;
		std::vector<std::string> enums;
		std::vector<std::string> enumFields;
		std::vector<std::string> functions;
		std::vector<std::string> fields;
		std::vector<std::string> globals;
		std::vector<std::string> methods;
		std::vector<std::string> namespaces;
		std::vector<std::string> structs;

	private:
		std::string addAccessPrefix(const std::string& str, AccessType access)
		{
			switch (access)
			{
			case ACCESS_PUBLIC:
				return "public " + str;
			case ACCESS_PROTECTED:
				return "protected " + str;
			case ACCESS_PRIVATE:
				return "private " + str;
			case ACCESS_NONE:
				return str;
			}
		}

		std::string addAbstractionPrefix(const std::string& str, AbstractionType abstraction)
		{
			switch (abstraction)
			{
			case ABSTRACTION_VIRTUAL:
				return "virtual " + str;
			case ABSTRACTION_PURE_VIRTUAL:
				return "pure virtual " + str;
			case ABSTRACTION_NONE:
				return str;
			}
		}

		std::string addStaticPrefix(const std::string& str, bool isStatic)
		{
			if (isStatic)
			{
				return "static " + str;
			}
			return str;
		}

		std::string addConstPrefix(const std::string& str, bool isConst, bool atFront)
		{
			if (isConst)
			{
				return atFront ? "const " + str : str + " const";
			}
			return str;
		}

		std::string variableStr(const ParseVariable& variable)
		{
			std::string str = variable.typeName + " " + variable.fullName;
			return addStaticPrefix(addConstPrefix(str, variable.isConst, true), variable.isStatic);
		}

		std::string parameterStr(const std::vector<ParseVariable> parameters)
		{
			std::string str = "(";
			for (size_t i = 0; i < parameters.size(); i++)
			{
				str += variableStr(parameters[i]);
				if (i < parameters.size() - 1)
				{
					str += ", ";
				}
			}
			return str + ")";
		}
	};
};
