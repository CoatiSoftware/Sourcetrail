#include "cxxtest/TestSuite.h"

#include "data/parser/cxx/CxxParser.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseVariable.h"
#include "utility/text/TextAccess.h"

class CxxParserTestSuite: public CxxTest::TestSuite
{
public:
	void test_cxx_parser_finds_global_class_definition()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class A\n"
			"{\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A");
	}

	void test_cxx_parser_finds_global_class_forward_declaration()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class A;\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A");
	}

	void test_cxx_parser_finds_nested_class_definition() // TODO: test different access types here
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class A\n"
			"{\n"
			"public:\n"
			"	class B;\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "A");
		TS_ASSERT_EQUALS(client->classes[1], "public A::B");
	}

	void test_cxx_parser_finds_class_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace a\n"
			"{\n"
			"	class B;\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "a::B");
	}

	void test_cxx_parser_finds_global_struct_definition()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"struct A\n"
			"{\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A");
	}

	void test_cxx_parser_finds_global_struct_forward_declaration()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"struct A;\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A");
	}

	void test_cxx_parser_finds_struct_definition_in_class()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class A\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "private A::B");
	}

	void test_cxx_parser_finds_struct_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace A\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A::B");
	}

	void test_cxx_parser_finds_variable_definitions_in_global_scope()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"int x;\n"
			"class A;\n"
			"A* b;";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->globals.size(), 2);
		TS_ASSERT_EQUALS(client->globals[0], "int x");
		TS_ASSERT_EQUALS(client->globals[1], "A * b");
	}

	void test_cxx_parser_finds_variable_definitions_in_namespace_scope()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace n"
			"{\n"
			"	int x;\n"
			"	class A;\n"
			"	A* b;\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->globals.size(), 2);
		TS_ASSERT_EQUALS(client->globals[0], "int n::x");
		TS_ASSERT_EQUALS(client->globals[1], "n::A * n::b");
	}

	void test_cxx_parser_finds_field_in_nested_class()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"public:\n"
			"	class C\n"
			"	{\n"
			"	private:\n"
			"		static const int amount;\n"
			"	};\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private static const int B::C::amount");
	}

	void test_cxx_parser_finds_field_in_global_class()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"public:\n"
			"	B() : count(0) {};"
			"private:\n"
			"	const int count;"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private const int B::count");
	}

	void test_cxx_parser_finds_function_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"int ceil(float a)\n"
			"{\n"
			"	return static_cast<int>(a) + 1;\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "int ceil(float a)");
	}

	void test_cxx_parser_finds_function_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace\n"
			"{\n"
			"	int sum(int a, int b);\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "int (anonymous namespace)::sum(int a, int b)");
	}

	void test_cxx_parser_finds_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"public:\n"
			"	B();\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B()");
	}

	void test_cxx_parser_finds_method_declaration_and_definition()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"public:\n"
			"	B();\n"
			"};\n"
			"B::B()\n"
			"{\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->methods.size(), 2);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B()");
		TS_ASSERT_EQUALS(client->methods[1], "public void B::B()");
	}

	void test_cxx_parser_finds_pure_virtual_method()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"protected:\n"
			"	virtual void process() = 0;\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "protected pure virtual void B::process()");
	}

	void test_cxx_parser_finds_method_declared_in_nested_class()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"	class C\n"
			"	{\n"
			"		bool isGreat() const;\n"
			"	};\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "private _Bool B::C::isGreat() const");
	}

	void test_cxx_parser_finds_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace A\n"
			"{"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->namespaces.size(), 1);
		TS_ASSERT_EQUALS(client->namespaces[0], "A");
	}

	void test_cxx_parser_finds_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace\n"
			"{\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->namespaces.size(), 1);
		TS_ASSERT_EQUALS(client->namespaces[0], "(anonymous)");
	}

	void test_cxx_parser_finds_enum_defined_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"enum E\n"
			"{\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "E");
	}

	void test_cxx_parser_finds_enum_defined_in_class()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"class B\n"
			"{\n"
			"public:\n"
			"	enum Z\n"
			"	{\n"
			"	};\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "public B::Z");
	}

	void test_cxx_parser_finds_enum_defined_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace n\n"
			"{\n"
			"	enum Z\n"
			"	{\n"
			"	};\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "n::Z");
	}

	void test_cxx_parser_finds_enum_field_in_global_enum()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"enum E\n"
			"{\n"
			"	P\n"
			"};\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->enumFields.size(), 1);
		TS_ASSERT_EQUALS(client->enumFields[0], "E::P");
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

	void test_cxx_parser_finds_typedef_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		std::string text =
			"namespace\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n";

		parser.parseFile(TextAccess::createFromString(text));

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "unsigned int -> (anonymous namespace)::uint");
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

	void test_cxx_parser_parses_multiple_files()
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);

		std::vector<std::string> filePaths;
		filePaths.push_back("data/CxxParserTestSuite/header.h");
		filePaths.push_back("data/CxxParserTestSuite/code.cpp");
		parser.parseFiles(filePaths);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->classes.size(), 3);
		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enumFields.size(), 2);
		TS_ASSERT_EQUALS(client->functions.size(), 2);
		TS_ASSERT_EQUALS(client->fields.size(), 4);
		TS_ASSERT_EQUALS(client->globals.size(), 2);
		TS_ASSERT_EQUALS(client->methods.size(), 5);
		TS_ASSERT_EQUALS(client->namespaces.size(), 2);
		TS_ASSERT_EQUALS(client->structs.size(), 1);
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
