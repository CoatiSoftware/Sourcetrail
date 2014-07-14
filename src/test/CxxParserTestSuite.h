#include "cxxtest/TestSuite.h"

#include <sstream>

#include "data/parser/cxx/CxxParser.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "utility/text/TextAccess.h"

class CxxParserTestSuite: public CxxTest::TestSuite
{
public:
	void test_cxx_parser_finds_global_class_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A <1:1 3:1>");
	}

	void test_cxx_parser_finds_global_class_forward_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A;\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "A <1:1 1:7>");
	}

	void test_cxx_parser_finds_nested_class_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	class B;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "A <1:1 5:1>");
		TS_ASSERT_EQUALS(client->classes[1], "public A::B <4:2 4:8>");
	}

	void test_cxx_parser_finds_class_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace a\n"
			"{\n"
			"	class B;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "a::B <3:2 3:8>");
	}

	void test_cxx_parser_finds_global_struct_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A <1:1 3:1>");
	}

	void test_cxx_parser_finds_global_struct_forward_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A;\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A <1:1 1:8>");
	}

	void test_cxx_parser_finds_struct_definition_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "private A::B <3:2 5:2>");
	}

	void test_cxx_parser_finds_struct_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 1);
		TS_ASSERT_EQUALS(client->structs[0], "A::B <3:2 5:2>");
	}

	void test_cxx_parser_finds_variable_definitions_in_global_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int x;\n"
			"const int y = 0;\n"
			"static int z;\n"
			"class A;\n"
			"A* b;"
		);

		TS_ASSERT_EQUALS(client->globalVariables.size(), 4);
		TS_ASSERT_EQUALS(client->globalVariables[0], "int x <1:1 1:5>");
		TS_ASSERT_EQUALS(client->globalVariables[1], "int const y <2:1 2:15>");
		TS_ASSERT_EQUALS(client->globalVariables[2], "static int z <3:1 3:12>");
		TS_ASSERT_EQUALS(client->globalVariables[3], "A * b <5:1 5:4>");
	}

	void test_cxx_parser_finds_variable_definitions_in_namespace_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n"
			"{\n"
			"	int x;\n"
			"	class A;\n"
			"	A* b;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->globalVariables.size(), 2);
		TS_ASSERT_EQUALS(client->globalVariables[0], "int n::x <2:2 2:6>");
		TS_ASSERT_EQUALS(client->globalVariables[1], "n::A * n::b <4:2 4:5>");
	}

	void test_cxx_parser_finds_field_in_nested_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	class C\n"
			"	{\n"
			"	private:\n"
			"		static const int amount;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private static int const B::C::amount <7:3 7:20>");
	}

	void test_cxx_parser_finds_fields_in_class_with_access_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	int a;"
			"public:\n"
			"	A() : d(0) {};\n"
			"	int b;"
			"protected:\n"
			"	static int c;\n"
			"private:\n"
			"	const int d;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 4);
		TS_ASSERT_EQUALS(client->fields[0], "private int A::a <3:2 3:6>");
		TS_ASSERT_EQUALS(client->fields[1], "public int A::b <5:2 5:6>");
		TS_ASSERT_EQUALS(client->fields[2], "protected static int A::c <6:2 6:13>");
		TS_ASSERT_EQUALS(client->fields[3], "private int const A::d <8:2 8:12>");
	}

	void test_cxx_parser_finds_function_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int ceil(float a)\n"
			"{\n"
			"	return static_cast<int>(a) + 1;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "int ceil(float) <1:1 4:1>");
	}

	void test_cxx_parser_finds_function_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	int sum(int a, int b);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->functions.size(), 1);
		TS_ASSERT_EQUALS(client->functions[0], "int (anonymous namespace)::sum(int, int) <3:2 3:22>");
	}

	void test_cxx_parser_finds_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	B();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B() <4:2 4:4>");
	}

	void test_cxx_parser_finds_method_declaration_and_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	B();\n"
			"};\n"
			"B::B()\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 2);
		TS_ASSERT_EQUALS(client->methods[0], "public void B::B() <4:2 4:4>");
		TS_ASSERT_EQUALS(client->methods[1], "public void B::B() <6:1 8:1>");
	}

	void test_cxx_parser_finds_pure_virtual_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"protected:\n"
			"	virtual void process() = 0;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "protected pure virtual void B::process() <4:2 4:27>");
	}

	void test_cxx_parser_finds_method_declared_in_nested_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"	class C\n"
			"	{\n"
			"		bool isGreat() const;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "private _Bool B::C::isGreat() const <5:3 5:18>");
	}

	void test_cxx_parser_finds_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->namespaces.size(), 1);
		TS_ASSERT_EQUALS(client->namespaces[0], "A <1:1 3:1>");
	}

	void test_cxx_parser_finds_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->namespaces.size(), 1);
		TS_ASSERT_EQUALS(client->namespaces[0], "(anonymous) <1:1 3:1>");
	}

	void test_cxx_parser_finds_enum_defined_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum E\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "E <1:1 3:1>");
	}

	void test_cxx_parser_finds_enum_defined_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	enum Z\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "public B::Z <4:2 6:2>");
	}

	void test_cxx_parser_finds_enum_defined_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n\n"
			"{\n"
			"	enum Z\n"
			"	{\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "n::Z <3:2 5:2>");
	}

	void test_cxx_parser_finds_enum_field_in_global_enum()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum E\n"
			"{\n"
			"	P\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->enumFields.size(), 1);
		TS_ASSERT_EQUALS(client->enumFields[0], "E::P <3:2 3:2>");
	}

	void test_cxx_parser_finds_typedef_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "unsigned int -> uint <1:1 1:22>");
	}

	void test_cxx_parser_finds_typedef_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace test\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "unsigned int -> test::uint <3:2 3:23>");
	}

	void test_cxx_parser_finds_typedef_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "unsigned int -> (anonymous namespace)::uint <3:2 3:23>");
	}

	void test_cxx_parser_finds_typedef_that_uses_type_defined_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace test\n"
			"{\n"
			"	struct TestStruct{};\n"
			"}\n"
			"typedef test::TestStruct globalTestStruct;\n"
		);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->typedefs[0], "test::TestStruct -> globalTestStruct <5:1 5:26>");
	}

	void test_cxx_parser_finds_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : public A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : public A <2:11 2:18>");
	}

	void test_cxx_parser_finds_protected_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : protected A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : protected A <2:11 2:21>");
	}

	void test_cxx_parser_finds_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : private A {};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "B : private A <2:11 2:19>");
	}

	void test_cxx_parser_finds_multiple_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B {};\n"
			"class C\n"
			"	: public A\n"
			"	, private B\n"
			"{};\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 2);
		TS_ASSERT_EQUALS(client->inheritances[0], "C : public A <4:4 4:11>");
		TS_ASSERT_EQUALS(client->inheritances[1], "C : private B <5:4 5:12>");
	}

	void test_cxx_parser_finds_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	sum(1, 2);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "main -> sum <7:2 7:10>");
	}

	void test_cxx_parser_finds_call_within_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	return sum(1, sum(2, 3));\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "main -> sum <7:9 7:25>");
		TS_ASSERT_EQUALS(client->calls[1], "main -> sum <7:16 7:24>");
	}

	void test_cxx_parser_finds_call_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"class App\n"
			"{\n"
			"	int main()\n"
			"	{\n"
			"		return sum(1, 2);\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "App::main -> sum <9:10 9:18>");
	}

	void test_cxx_parser_finds_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "main -> App::App <8:6 8:6>");
	}

	void test_cxx_parser_finds_constructor_without_definition_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "main -> App::App <6:6 6:6>");
	}

	void test_cxx_parser_finds_constructor_call_of_field()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Item\n"
			"{\n"
			"};\n"
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"	Item item;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "App::App -> Item::Item <7:2 7:2>");
	}

	void test_cxx_parser_finds_constructor_call_of_field_in_initialization_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Item\n"
			"{\n"
			"public:\n"
			"	Item(int n) {}\n"
			"};\n"
			"class App\n"
			"{\n"
			"	App()\n"
			"		: item(1)"
			"	{}\n"
			"	Item item;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "App::App -> Item::Item <9:5 9:11>");
	}

	void test_cxx_parser_finds_function_call_within_constructor_call_of_field_in_initialization_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int one() { return 1; }\n"
			"class Item\n"
			"{\n"
			"public:\n"
			"	Item(int n) {}\n"
			"};\n"
			"class App\n"
			"{\n"
			"	App()\n"
			"		: item(one())"
			"	{}\n"
			"	Item item;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "App::App -> Item::Item <10:5 10:15>");
		TS_ASSERT_EQUALS(client->calls[1], "App::App -> one <10:10 10:14>");
	}

	void test_cxx_parser_finds_copy_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"	App(const App& other) {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"	App app2(app);\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "main -> App::App <9:6 9:6>");
		TS_ASSERT_EQUALS(client->calls[1], "main -> App::App <10:6 10:14>");
	}

	void test_cxx_parser_finds_global_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"};\n"
			"App app;\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "app -> App::App <6:5 6:5>");
	}

	void test_cxx_parser_finds_global_function_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int one() { return 1; }\n"
			"int a = one();\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "a -> one <2:9 2:13>");
	}

	void test_cxx_parser_finds_operator_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	void operator+(int a)\n"
			"	{\n"
			"	}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App app;\n"
			"	app + 2;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 2);
		TS_ASSERT_EQUALS(client->calls[0], "main -> App::App <10:6 10:6>");
		TS_ASSERT_EQUALS(client->calls[1], "main -> App::operator+ <11:2 11:8>");
	}

	void test_cxx_parser_finds_return_type_use_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"double PI()\n"
			"{\n"
			"	return 3.14159265359;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "double <1:1 1:6>");
	}

	void test_cxx_parser_finds_return_and_parameter_type_uses_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int ceil(float a)\n"
			"{\n"
			"	return static_cast<int>(a) + 1;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "int <1:1 1:3>");
		TS_ASSERT_EQUALS(client->typeUses[1], "float <1:10 1:16>");
	}

	void test_cxx_parser_finds_parameter_type_uses_in_constructor()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	A(int a, bool b, float c, int d);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 4);
		TS_ASSERT_EQUALS(client->typeUses[0], "int <3:4 3:8>");
		TS_ASSERT_EQUALS(client->typeUses[1], "_Bool <3:11 3:16>");
		TS_ASSERT_EQUALS(client->typeUses[2], "float <3:19 3:25>");
		TS_ASSERT_EQUALS(client->typeUses[3], "int <3:28 3:32>");
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
		TS_ASSERT_EQUALS(client->globalVariables.size(), 2);
		TS_ASSERT_EQUALS(client->methods.size(), 5);
		TS_ASSERT_EQUALS(client->namespaces.size(), 2);
		TS_ASSERT_EQUALS(client->structs.size(), 1);
	}

private:
	class TestParserClient: public ParserClient
	{
	public:
		virtual void onTypedefParsed(
			const ParseLocation& location, const std::string& fullName, const DataType& underlyingType,
			AccessType access
		)
		{
			std::string str = addAccessPrefix(underlyingType.getFullTypeName() + " -> " + fullName, access);
			typedefs.push_back(addLocationSuffix(str, location));
		}

		virtual void onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
		{
			classes.push_back(addLocationSuffix(addAccessPrefix(fullName, access), location));
		}

		virtual void onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
		{
			structs.push_back(addLocationSuffix(addAccessPrefix(fullName, access), location));
		}

		virtual void onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
		{
			globalVariables.push_back(addLocationSuffix(variableStr(variable), location));
		}

		virtual void onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
		{
			fields.push_back(addLocationSuffix(addAccessPrefix(variableStr(variable), access), location));
		}

		virtual void onFunctionParsed(
			const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
			const std::vector<ParseTypeUsage>& parameters
		){
			std::string str = functionStr(returnType.type, fullName, parameters, false);
			functions.push_back(addLocationSuffix(str, location));

			addTypeUse(returnType);
			for (const ParseTypeUsage& parameter : parameters)
			{
				addTypeUse(parameter);
			}
		}

		virtual void onMethodParsed(
			const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
			const std::vector<ParseTypeUsage>& parameters, AccessType access, AbstractionType abstraction,
			bool isConst, bool isStatic
		)
		{
			std::string str = functionStr(returnType.type, fullName, parameters, isConst);
			str = addStaticPrefix(addAbstractionPrefix(str, abstraction), isStatic);
			str = addAccessPrefix(str, access);
			str = addLocationSuffix(str, location);
			methods.push_back(str);

			addTypeUse(returnType);
			for (const ParseTypeUsage& parameter : parameters)
			{
				addTypeUse(parameter);
			}
		}

		virtual void onNamespaceParsed(const ParseLocation& location, const std::string& fullName)
		{
			namespaces.push_back(addLocationSuffix(fullName, location));
		}

		virtual void onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
		{
			enums.push_back(addLocationSuffix(addAccessPrefix(fullName, access), location));
		}

		virtual void onEnumFieldParsed(const ParseLocation& location, const std::string& fullName)
		{
			enumFields.push_back(addLocationSuffix(fullName, location));
		}

		virtual void onInheritanceParsed(
			const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access)
		{
			std::string str = fullName + " : " + addAccessPrefix(baseName, access);
			inheritances.push_back(addLocationSuffix(str, location));
		}

		virtual void onCallParsed(
			const ParseLocation& location, const std::string& callerName, const std::string& calleeName)
		{
			calls.push_back(addLocationSuffix(callerName + " -> " + calleeName, location));
		}

		std::vector<std::string> typedefs;
		std::vector<std::string> classes;
		std::vector<std::string> enums;
		std::vector<std::string> enumFields;
		std::vector<std::string> functions;
		std::vector<std::string> fields;
		std::vector<std::string> globalVariables;
		std::vector<std::string> methods;
		std::vector<std::string> namespaces;
		std::vector<std::string> structs;
		std::vector<std::string> inheritances;
		std::vector<std::string> calls;
		std::vector<std::string> typeUses;

	private:
		void addTypeUse(const ParseTypeUsage& use)
		{
			if (use.location.isValid())
			{
				typeUses.push_back(addLocationSuffix(use.type.getFullTypeName(), use.location));
			}
		}
	};

	std::shared_ptr<TestParserClient> parseCode(std::string code) const
	{
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client);
		parser.parseFile(TextAccess::createFromString(code));
		return client;
	}
};
