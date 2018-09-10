#include "cxxtest/TestSuite.h"

#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/indexer/IndexerCommandCxx.h"
#include "data/parser/cxx/CxxParser.h"

#include "helper/TestFileRegister.h"
#include "helper/TestParserClient.h"

class CxxParserTestSuite: public CxxTest::TestSuite
{
public:
	void test_cxx_parser_finds_usage_of_field_in_function_call_arguments()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	void foo(int i)\n"
			"	{\n"
			"		foo(bar);\n"
			"	}\n"
			"	int bar;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A::foo(int) -> int A::bar <6:7 6:9>"
		));
	}

	void test_cxx_parser_usage_of_field_in_function_call_context()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	void foo(int i)\n"
			"	{\n"
			"		a->foo(6);\n"
			"	}\n"
			"	A* a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A::foo(int) -> A * A::a <6:3 6:3>"
		));
	}

///////////////////////////////////////////////////////////////////////////////
// test finding symbol definitions and declarations

	void test_cxx_parser_finds_global_variable_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int x;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"int x <1:5 1:5>"
		));
	}

	void test_cxx_parser_finds_static_global_variable_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"static int x;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"int x (input.cc) <1:12 1:12>"
		));
	}

	void test_cxx_parser_finds_static_const_global_variable_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"static const int x;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"const int x (input.cc) <1:18 1:18>"
		));
	}

	void test_cxx_parser_finds_global_class_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"A <1:1 <1:7 1:7> 3:1>"
		));
	}

	void test_cxx_parser_finds_global_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"A <1:7 1:7>"
		));
	}

	void test_cxx_parser_finds_global_struct_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"A <1:1 <1:8 1:8> 3:1>"
		));
	}

	void test_cxx_parser_finds_global_struct_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"A <1:8 1:8>"
		));
	}

	void test_cxx_parser_finds_variable_definitions_in_global_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int x;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"int x <1:5 1:5>"
		));
	}

	void test_cxx_parser_finds_fields_in_class_with_access_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	int a;\n"
			"public:\n"
			"	A() : d(0) {};\n"
			"	int b;\n"
			"protected:\n"
			"	static int c;\n"
			"private:\n"
			"	const int d;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private int A::a <3:6 3:6>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"public int A::b <6:6 6:6>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"protected static int A::c <8:13 8:13>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private const int A::d <10:12 10:12>"
		));
	}

	void test_cxx_parser_finds_function_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int ceil(float a)\n"
			"{\n"
			"	return 1;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->functions, L"int ceil(float) <1:1 <1:1  <1:5 1:8> 1:17> 4:1>"
		));
	}

	void test_cxx_parser_finds_static_function_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"static int ceil(float a)\n"
			"{\n"
			"	return static_cast<int>(a) + 1;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->functions, L"static int ceil(float) (input.cc) <1:1 <1:1  <1:12 1:15> 1:24> 4:1>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public void B::B() <4:2 <4:2 4:2> 4:4>"
		));
	}

	void test_cxx_parser_finds_overloaded_operator_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	B& operator=(const B& other);\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public B & B::operator=(const B &) <4:2 <4:5 4:13> 4:29>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public void B::B() <6:1 <6:4 6:4> 8:1>"
		));
	}

	void test_cxx_parser_finds_virtual_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"public:\n"
			"	virtual void process();\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public void B::process() <4:2 <4:15 4:21> 4:23>"
		));
	}

	void test_cxx_parser_finds_pure_virtual_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class B\n"
			"{\n"
			"protected:\n"
			"	virtual void process() = 0;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"protected void B::process() <4:2 <4:15 4:21> 4:27>"
		));
	}

	void test_cxx_parser_finds_named_namespace_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"A <1:1 <1:11 1:11> 3:1>"
		));
	}

	void test_cxx_parser_finds_anonymous_namespace_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"anonymous namespace (input.cc<1:1>) <1:1 <2:1 2:1> 3:1>"
		));
	}

	void test_cxx_parser_finds_multiple_anonymous_namespace_declarations_as_same_symbol()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"}\n"
			"namespace\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"anonymous namespace (input.cc<1:1>) <1:1 <2:1 2:1> 3:1>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"anonymous namespace (input.cc<1:1>) <4:1 <5:1 5:1> 6:1>"
		));
	}

	void test_cxx_parser_finds_multiple_nested_anonymous_namespace_declarations_as_different_symbol()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	namespace\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"anonymous namespace (input.cc<1:1>) <1:1 <2:1 2:1> 6:1>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"anonymous namespace (input.cc<1:1>)::anonymous namespace (input.cc<3:2>) <3:2 <4:2 4:2> 5:2>"
		));
	}

	void test_cxx_parser_finds_anonymous_namespace_declarations_nested_inside_namespaces_with_different_name_as_different_symbol()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace a\n"
			"{\n"
			"	namespace\n"
			"	{\n"
			"	}\n"
			"}\n"
			"namespace b\n"
			"{\n"
			"	namespace\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"a <1:1 <1:11 1:11> 6:1>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"a::anonymous namespace (input.cc<3:2>) <3:2 <4:2 4:2> 5:2>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"b <7:1 <7:11 7:11> 12:1>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"b::anonymous namespace (input.cc<9:2>) <9:2 <10:2 10:2> 11:2>"
		));
	}

	void test_cxx_parser_finds_anonymous_namespace_declarations_nested_inside_namespaces_with_same_name_as_same_symbol()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace a\n"
			"{\n"
			"	namespace\n"
			"	{\n"
			"	}\n"
			"}\n"
			"namespace a\n"
			"{\n"
			"	namespace\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"a <1:1 <1:11 1:11> 6:1>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"a::anonymous namespace (input.cc<3:2>) <3:2 <4:2 4:2> 5:2>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"a <7:1 <7:11 7:11> 12:1>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"a::anonymous namespace (input.cc<3:2>) <9:2 <10:2 10:2> 11:2>"
		));
	}

	void test_cxx_parser_finds_anonymous_struct_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef struct\n"
			"{\n"
			"	int x;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"anonymous struct (input.cc<1:9>) <1:9 <1:9 1:14> 4:1>"
		));
	}

	void test_cxx_parser_finds_multiple_anonymous_struct_declarations_as_distinct_elements()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef struct\n"
			"{\n"
			"	int x;\n"
			"};\n"
			"typedef struct\n"
			"{\n"
			"	float x;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->structs.size(), 2);
		TS_ASSERT_EQUALS(client->fields.size(), 2);
		TS_ASSERT_DIFFERS(utility::substrBeforeLast(client->fields[0], '<'), utility::substrBeforeLast(client->fields[1], '<'));
	}

	void test_cxx_parser_finds_anonymous_union_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef union\n"
			"{\n"
			"	int i;\n"
			"	float f;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->unions, L"anonymous union (input.cc<1:9>) <1:9 <1:9 1:13> 5:1>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_struct_declared_inside_typedef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef struct\n"
			"{\n"
			"	int x;\n"
			"} Foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"Foo <1:9 <1:9 1:14> 4:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"Foo <4:3 4:5>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_class_declared_inside_typedef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef class\n"
			"{\n"
			"	int x;\n"
			"} Foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"Foo <1:9 <1:9 1:13> 4:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"Foo <4:3 4:5>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_enum_declared_inside_typedef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef enum\n"
			"{\n"
			"	CONSTANT_1;\n"
			"} Foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"Foo <1:9 <1:9 1:12> 4:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"Foo <4:3 4:5>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_union_declared_inside_typedef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef union\n"
			"{\n"
			"	int x;\n"
			"	float y;\n"
			"} Foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->unions, L"Foo <1:9 <1:9 1:13> 5:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->unions, L"Foo <5:3 5:5>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_struct_declared_inside_type_alias()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"using Foo = struct\n"
			"{\n"
			"	int x;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"Foo <1:13 <1:13 1:18> 4:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"Foo <1:7 1:9>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_class_declared_inside_type_alias()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"using Foo = class\n"
			"{\n"
			"	int x;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"Foo <1:13 <1:13 1:17> 4:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"Foo <1:7 1:9>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_enum_declared_inside_type_alias()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"using Foo = enum\n"
			"{\n"
			"	CONSTANT_1;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"Foo <1:13 <1:13 1:16> 4:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"Foo <1:7 1:9>"
		));
	}

	void test_cxx_parser_finds_name_of_anonymous_union_declared_inside_type_alias()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"using Foo = union\n"
			"{\n"
			"	int x;\n"
			"	float y;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->unions, L"Foo <1:13 <1:13 1:17> 5:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->unions, L"Foo <1:7 1:9>"
		));
	}

	void test_cxx_parser_finds_enum_defined_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum E\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"E <1:1 <1:6 1:6> 3:1>"
		));
	}

	void test_cxx_parser_finds_enum_constant_in_global_enum()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum E\n"
			"{\n"
			"	P\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enumConstants, L"E::P <3:2 3:2>"
		));
	}

	void test_cxx_parser_finds_typedef_in_global_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typedefs, L"uint <1:22 1:25>"
		));
	}

	void test_cxx_parser_finds_typedef_in_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace test\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typedefs, L"test::uint <3:23 3:26>"
		));
	}

	void test_cxx_parser_finds_typedef_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	typedef unsigned int uint;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typedefs, L"anonymous namespace (input.cc<1:1>)::uint <3:23 3:26>"
		));
	}

	void test_cxx_parser_finds_type_alias_in_class()
	{
			std::shared_ptr<TestParserClient> client = parseCode(
			"class Foo\n"
			"{\n"
			"	using Bar = Foo;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typedefs, L"private Foo::Bar <3:8 3:10>"
		));
	}

	void test_cxx_parser_finds_macro_define()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macros, L"PI <1:9 <1:9 1:10> 1:8>"
		));
	}

	void test_cxx_parser_finds_macro_undefine()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#undef PI\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macroUses, L"input.cc -> PI <1:8 1:9>"
		));
	}

	void test_cxx_parser_finds_macro_in_ifdef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI\n"
			"#ifdef PI\n"
			"void test()\n"
			"{\n"
			"};\n"
			"#endif\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macroUses, L"input.cc -> PI <2:8 2:9>"
		));
	}

	void test_cxx_parser_finds_macro_in_ifndef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI\n"
			"#ifndef PI\n"
			"void test()\n"
			"{\n"
			"};\n"
			"#endif\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macroUses, L"input.cc -> PI <2:9 2:10>"
		));
	}

	void test_cxx_parser_finds_macro_in_ifdefined()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI\n"
			"#if defined(PI)\n"
			"void test()\n"
			"{\n"
			"};\n"
			"#endif\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macroUses, L"input.cc -> PI <2:13 2:14>"
		));
	}

	void test_cxx_parser_finds_macro_expand()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI 3.14159265359\n"
			"void test()\n"
			"{\n"
			"double i = PI;"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macroUses, L"input.cc -> PI <4:12 4:13>"
		));
	}

	void test_cxx_parser_finds_macro_expand_within_macro()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define PI 3.14159265359\n"
			"#define TAU (2 * PI)\n"
			"void test()\n"
			"{\n"
			"double i = TAU;"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macroUses, L"input.cc -> PI <2:18 2:19>"
		));
	}

	void test_cxx_parser_finds_macro_define_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define MAX(a,b) \\\n"
			"	((a)>(b)?(a):(b))"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->macros, L"MAX <1:9 <1:9 1:11> 2:17>"
		));
	}

	//void __test_cxx_parser_finds_type_template_parameter_type_of_template_type_alias()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"template<class T>\n"
	//		"using MyType = int;\n"
	//	);

	//	TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 1);
	//	TS_ASSERT_EQUALS(client->templateParameterTypes[0], L"MyType<class T>::T <1:17 1:17>");
	//}

	void test_cxx_parser_finds_type_template_parameter_type_of_class_template()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename T>::T <1:20 1:20>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T>\n"
			"class A<T, int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename T, int>::T <5:20 5:20>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_type_of_variable_template()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T v;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"v<typename T>::T <1:20 1:20>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_of_explicit_partial_variable_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename Q>\n"
			"T t = Q(5);\n"
			"\n"
			"template <typename R>\n"
			"int t<int, R> = 9;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"t<int, typename R>::R <4:20 4:20>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_defined_with_class_keyword()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <class T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<class T>::T <1:17 1:17>"
		));
	}

	void test_cxx_parser_finds_non_type_int_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<int T>::T <1:15 1:15>"
		));
	}

	void test_cxx_parser_finds_non_type_bool_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<bool T>::T <1:16 1:16>"
		));
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<P * p>::p <3:14 3:14>"
		));
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<P & p>::p <3:14 3:14>"
		));
	}

	void test_cxx_parser_finds_non_type_template_parameter_that_depends_on_type_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T1, T1& T2>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename T1, T1 & T2>::T2 <1:28 1:29>"
		));
	}

	void test_cxx_parser_finds_non_type_template_parameter_that_depends_on_template_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <template<typename> class T1, T1<int>& T2>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<template<typename> typename T1, T1<int> & T2>::T2 <1:49 1:50>"
		));
	}

	void test_cxx_parser_finds_non_type_template_parameter_that_depends_on_type_template_parameter_of_template_template_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <template<typename T, T R>typename S>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<template<typename T, T R> typename S>::S<typename T, T R> <1:45 1:45>"
		));
	}

	void test_cxx_parser_finds_template_argument_of_dependent_non_type_template_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <template<typename> class T1, T1<int>& T2>\n"
			"class A\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<template<typename> typename T1, T1<int> & T2>::T1<int> -> int <1:43 1:45>"
		));
	}

	void test_cxx_parser_finds_template_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{};\n"
			"int main()\n"
			"{\n"
			"	B<A> ba;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"B<template<typename> typename T>::T<typename> <4:36 4:36>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_pack_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename... T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename... T>::T <1:23 1:23>"
		));
	}

	void test_cxx_parser_finds_non_type_int_template_parameter_pack_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int... T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<int... T>::T <1:18 1:18>"
		));
	}

	void test_cxx_parser_finds_template_template_parameter_pack_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <template<typename> typename... T>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<template<typename> typename... T>::T<typename> <1:42 1:42>"
		));
	}

	void test_cxx_parser_finds_type_template_parameters_of_template_class_with_multiple_parameters()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename T, typename U>::T <1:20 1:20>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename T, typename U>::U <1:32 1:32>"
		));
	}

	void test_cxx_parser_skips_creating_node_for_template_parameter_without_a_name()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateParameterTypes.size(), 0);
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"A<typename> <1:1 <2:7 2:7> 4:1>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_of_template_method_definition_outside_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	template <typename U>\n"
			"	U foo();\n"
			"};\n"
			"template <typename T>\n"
			"template <typename U>\n"
			"U A<T>::foo()\n"
			"{}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"A<typename T>::foo<typename U>::U <8:20 8:20>"
		));
	}

	void test_cxx_parser_finds_explicit_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"A<int> <5:1 <6:7 6:7> 8:1>"
		));
	}

	void test_cxx_parser_finds_explicit_variable_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T t = T(5);\n"
			"\n"
			"template <>\n"
			"int t<int> = 99;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"int t<int> <5:5 5:5>"
		));
	}

	void test_cxx_parser_finds_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T>\n"
			"class A<T, int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"A<typename T, int> <5:1 <6:7 6:7> 8:1>"
		));
	}

	void test_cxx_parser_finds_explicit_partial_variable_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename Q>\n"
			"T t = Q(5);\n"
			"\n"
			"template <typename R>\n"
			"int t<int, R> = 9;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"int t<int, typename R> <5:5 5:5>"
		));
	}

	void test_cxx_parser_finds_correct_field_member_name_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	int foo;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private int A<typename T>::foo <4:6 4:8>"
		));
	}

	void test_cxx_parser_finds_correct_type_of_field_member_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<typename T>::T A<typename T>::foo -> A<typename T>::T <4:2 4:2>"
		));
	}

	void test_cxx_parser_finds_correct_method_member_name_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	int foo();\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"private int A<typename T>::foo() <4:2 <4:6 4:8> 4:10>"
		));
	}

	void test_cxx_parser_finds_type_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"test<typename T>::T <1:20 1:20>"
		));
	}

	void test_cxx_parser_finds_non_type_int_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"int test(int a)\n"
			"{\n"
			"	return a + T;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"test<int T>::T <1:15 1:15>"
		));
	}

	void test_cxx_parser_finds_non_type_bool_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"int test(int a)\n"
			"{\n"
			"	return T ? a : 0;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"test<bool T>::T <1:16 1:16>"
		));
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"int test(int a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"test<P * p>::p <3:14 3:14>"
		));
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"int test(int a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"test<P & p>::p <3:14 3:14>"
		));
	}

	void test_cxx_parser_finds_template_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"int test(int a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"test<template<typename> typename T>::T<typename> <4:36 4:36>"
		));
	}

	void test_cxx_parser_finds_function_for_implicit_instantiation_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	return test(1);\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->functions, L"int test<int>(int) <2:1 <2:1  <2:3 2:6> 2:11> 5:1>"
		));
	}

	void test_cxx_parser_finds_lambda_definition_and_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void lambdaCaller()\n"
			"{\n"
			"	[](){}();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->functions, L"void lambdaCaller::lambda at 3:2() const <3:5 <3:2 3:2> 3:7>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void lambdaCaller() -> void lambdaCaller::lambda at 3:2() const <3:8 3:8>"
		));
	}

	void test_cxx_parser_finds_mutable_lambda_definition()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void lambdaWrapper()\n"
			"{\n"
			"	[](int foo) mutable { return foo; };\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->functions, L"int lambdaWrapper::lambda at 3:2(int) <3:14 <3:2 3:2> 3:36>"
		));
	}

	void test_cxx_parser_finds_local_variable_declared_in_lambda_capture()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void lambdaWrapper()\n"
			"{\n"
			"	[x(42)]() { return x; };\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:3> <3:3 3:3>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:3> <3:21 3:21>"
		));
	}

	void test_cxx_parser_finds_definition_of_local_symbol_in_function_parameter_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void test(int a)\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<1:15> <1:15 1:15>"
		));
	}

	void test_cxx_parser_finds_definition_of_local_symbol_in_function_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void test()\n"
			"{\n"
			"	int a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:6> <3:6 3:6>"
		));
	}

///////////////////////////////////////////////////////////////////////////////
// test finding nested symbol definitions and declarations

	void test_cxx_parser_finds_class_definition_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	class B;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"public A::B <4:8 4:8>"
		));
	}

	void test_cxx_parser_finds_class_definition_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace a\n"
			"{\n"
			"	class B;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"a::B <3:8 3:8>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"private A::B <3:2 <3:9 3:9> 5:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"A::B <3:2 <3:9 3:9> 5:2>"
		));
	}

	void test_cxx_parser_finds_struct_definition_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void foo(int)\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
			"void foo(float)\n"
			"{\n"
			"	struct B\n"
			"	{\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"foo::B <3:2 <3:9 3:9> 5:2>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->structs, L"foo::B <9:2 <9:9 9:9> 11:2>"
		));
	}

	void test_cxx_parser_finds_variable_definitions_in_namespace_scope()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n"
			"{\n"
			"	int x;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->globalVariables, L"int n::x <2:6 2:6>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private static const int B::C::amount <7:20 7:25>"
		));
	}

	void test_cxx_parser_finds_function_in_anonymous_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace\n"
			"{\n"
			"	int sum(int a, int b);\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->functions, L"int anonymous namespace (input.cc<1:1>)::sum(int, int) <3:2 <3:6 3:8> 3:22>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"private bool B::C::isGreat() const <5:3 <5:8 5:14> 5:22>"
		));
	}

	void test_cxx_parser_finds_nested_named_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace A\n"
			"{\n"
			"	namespace B\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->namespaces, L"A::B <3:2 <3:12 3:12> 5:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"public B::Z <4:2 <4:7 4:7> 6:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"n::Z <3:2 <3:7 3:7> 5:2>"
		));
	}

	void test_cxx_parser_finds_enum_definition_in_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	enum TestType\n"
			"	{\n"
			"		TEST_ONE,\n"
			"		TEST_TWO\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"private A<typename T>::TestType <4:2 <4:7 4:14> 8:2>"
		));
	}

	void test_cxx_parser_finds_enum_constants_in_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	enum TestType\n"
			"	{\n"
			"		TEST_ONE,\n"
			"		TEST_TWO\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enumConstants, L"A<typename T>::TestType::TEST_ONE <6:3 6:10>"
		));
	}

///////////////////////////////////////////////////////////////////////////////
// test qualifier locations

	void test_cxx_parser_finds_qualifier_of_access_to_global_variable_defined_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace foo {\n"
			"	namespace bar {\n"
			"		int x;\n"
			"	}\n"
			"}\n"
			"void f() {\n"
			"	foo::bar::x = 9;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"foo <7:2 7:4>"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"foo::bar <7:7 7:9>"));
	}

	void test_cxx_parser_finds_qualifier_of_access_to_static_field()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Foo {\n"
			"public:\n"
			"	struct Bar {\n"
			"	public:\n"
			"		static int x;\n"
			"	};\n"
			"};\n"
			"void f() {\n"
			"	Foo::Bar::x = 9;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"Foo <9:2 9:4>"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"Foo::Bar <9:7 9:9>"));
	}

	void test_cxx_parser_finds_qualifier_of_access_to_enum_constant()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum Foo {\n"
			"	FOO_V\n"
			"};\n"
			"void f() {\n"
			"	Foo v = Foo::FOO_V;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"Foo <5:10 5:12>"));
	}

	void test_cxx_parser_finds_qualifier_of_reference_to_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Foo {\n"
			"public:\n"
			"	static void my_int_func(int x) {\n"
			"	}\n"
			"};\n"
			"\n"
			"void test() {\n"
			"	void(*foo)(int);\n"
			"	foo = &Foo::my_int_func;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"Foo <9:9 9:11>"));
	}

	void test_cxx_parser_finds_qualifier_of_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Foo {\n"
			"public:\n"
			"	Foo(int i) {}\n"
			"};\n"
			"\n"
			"class Bar : public Foo {\n"
			"public:\n"
			"	Bar() : Foo::Foo(4) {}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->qualifiers, L"Foo <8:10 8:12>"));
	}

///////////////////////////////////////////////////////////////////////////////
// test implicit symbols

	void test_cxx_parser_finds_builtin_types()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void t1(int v) {}\n"
			"void t2(float v) {}\n"
			"void t3(double v) {}\n"
			"void t4(bool v) {}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->builtinTypes, L"void"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->builtinTypes, L"int"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->builtinTypes, L"float"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->builtinTypes, L"double"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->builtinTypes, L"bool"));
	}

	void test_cxx_parser_finds_implicit_copy_constructor()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class TestClass {}\n"
			"void foo()\n"
			"{\n"
			"	TestClass a;\n"
			"	TestClass b(a);\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(client->methods, L"public void TestClass::TestClass() <1:7 <1:7 1:15> 1:15>"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->methods, L"public void TestClass::TestClass(const TestClass &) <1:7 <1:7 1:15> 1:15>"));
		TS_ASSERT(utility::containsElement<std::wstring>(client->methods, L"public void TestClass::TestClass(TestClass &&) <1:7 <1:7 1:15> 1:15>"));
	}

///////////////////////////////////////////////////////////////////////////////
// test finding usages of symbols

	void test_cxx_parser_finds_enum_usage_in_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	enum TestType\n"
			"	{\n"
			"		TEST_ONE,\n"
			"		TEST_TWO\n"
			"	};\n"
			"	TestType foo;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<typename T>::TestType A<typename T>::foo -> A<typename T>::TestType <9:2 9:9>"
		));
	}

	void test_cxx_parser_finds_correct_field_member_type_of_nested_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	class B\n"
			"	{\n"
			"		T foo;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<typename T>::T A<typename T>::B::foo -> A<typename T>::T <6:3 6:3>"
		));
	}

	void test_cxx_parser_finds_type_usage_of_global_variable()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int x;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int x -> int <1:1 1:3>"
		));
	}

	void test_cxx_parser_finds_typedefs_type_use()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"uint -> unsigned int <1:9 1:16>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"globalTestStruct -> test::TestStruct <5:15 5:24>"
		));
	}

	void test_cxx_parser_finds_type_use_of_typedef()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"typedef unsigned int uint;\n"
			"uint number;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"uint number -> uint <2:1 2:4>"
		));
	}

	void test_cxx_parser_finds_class_default_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:11 2:11>"
		));
	}

	void test_cxx_parser_finds_class_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : public A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:18 2:18>"
		));
	}

	void test_cxx_parser_finds_class_protected_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : protected A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:21 2:21>"
		));
	}

	void test_cxx_parser_finds_class_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B : private A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:19 2:19>"
		));
	}

	void test_cxx_parser_finds_class_multiple_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {};\n"
			"class B {};\n"
			"class C\n"
			"	: public A\n"
			"	, private B\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"C -> A <4:11 4:11>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"C -> B <5:12 5:12>"
		));
	}

	void test_cxx_parser_finds_struct_default_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:12 2:12>"
		));
	}

	void test_cxx_parser_finds_struct_public_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : public A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:19 2:19>"
		));
	}

	void test_cxx_parser_finds_struct_protected_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : protected A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:22 2:22>"
		));
	}

	void test_cxx_parser_finds_struct_private_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B : private A {};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A <2:20 2:20>"
		));
	}

	void test_cxx_parser_finds_struct_multiple_inheritance()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"struct A {};\n"
			"struct B {};\n"
			"struct C\n"
			"	: public A\n"
			"	, private B\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"C -> A <4:11 4:11>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"C -> B <5:12 5:12>"
		));
	}

	void test_cxx_parser_finds_method_override_when_virtual()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	void foo();\n"
			"};"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->overrides, L"void B::foo() -> void A::foo() <5:7 5:9>"
		));
	}

	void test_cxx_parser_finds_multi_layer_method_overrides()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	void foo();\n"
			"};\n"
			"class C : public B {\n"
			"	void foo();\n"
			"};"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->overrides, L"void B::foo() -> void A::foo() <5:7 5:9>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->overrides, L"void C::foo() -> void B::foo() <8:7 8:9>"
		));
	}

	void test_cxx_parser_finds_method_overrides_on_different_return_types()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	int foo();\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->errors.size(), 1);
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->overrides, L"int B::foo() -> void A::foo() <5:6 5:8>"
		));
	}

	void test_cxx_parser_finds_no_method_override_when_not_virtual()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	void foo();\n"
			"};\n"
			"class B : public A {\n"
			"	void foo();\n"
			"};"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 0);
	}

	void test_cxx_parser_finds_no_method_overrides_on_different_signatures()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {\n"
			"	virtual void foo(int a);\n"
			"};\n"
			"class B : public A {\n"
			"	int foo(int a, int b);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 0);
	}

	void test_cxx_parser_finds_using_directive_decl_in_function_context()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void foo()\n"
			"{\n"
			"	using namespace std;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void foo() -> std <3:18 3:20>"
		));
	}

	void test_cxx_parser_finds_using_directive_decl_in_file_context()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"using namespace std;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"input.cc -> std <1:17 1:19>"
		));
	}

	void test_cxx_parser_finds_using_decl_in_function_context()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace foo\n"
			"{\n"
			"	int a;\n"
			"}\n"
			"void bar()\n"
			"{\n"
			"	using foo::a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void bar() -> foo::a <7:13 7:13>"
		));
	}

	void test_cxx_parser_finds_using_decl_in_file_context()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace foo\n"
			"{\n"
			"	int a;\n"
			"}\n"
			"using foo::a;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"input.cc -> foo::a <5:12 5:12>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> int sum(int, int) <7:2 7:4>"
		));
	}

	void test_cxx_parser_finds_call_in_function_with_correct_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"void func()\n"
			"{\n"
			"}\n"
			"void func(bool right)\n"
			"{\n"
			"	sum(1, 2);\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void func(bool) -> int sum(int, int) <10:2 10:4>"
		));
	}

	void test_cxx_parser_finds_call_to_function_with_right_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int sum(int a, int b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"float sum(float a, float b)\n"
			"{\n"
			"	return a + b;\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	sum(1, 2);\n"
			"	sum(1.0f, 0.5f);\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> int sum(int, int) <11:2 11:4>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> float sum(float, float) <12:2 12:4>"
		));
	}

	void test_cxx_parser_finds_function_call_in_function_parameter_list()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> int sum(int, int) <7:16 7:18>"
		));
	}

	void test_cxx_parser_finds_function_call_in_method()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int App::main() -> int sum(int, int) <9:10 9:12>"
		));
	}

	void test_cxx_parser_finds_implicit_constructor_without_definition_call()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> void App::App() <6:6 6:8>"
		));
	}

	void test_cxx_parser_finds_explicit_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> void App::App() <8:2 8:4>"
		));
	}

	void test_cxx_parser_finds_explicit_constructor_call_of_field()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Item\n"
			"{\n"
			"};\n"
			"class App\n"
			"{\n"
			"public:\n"
			"	App() : item() {}\n"
			"	Item item;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void App::App() -> void Item::Item() <7:10 7:13>"
		));
	}

	void test_cxx_parser_finds_function_call_in_member_initialization()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void App::App() -> int one() <10:10 10:12>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> void App::App(const App &) <10:6 10:9>"
		));
	}

	void test_cxx_parser_finds_global_variable_constructor_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App() {}\n"
			"};\n"
			"App app;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"App app -> void App::App() <6:5 6:7>"
		));
	}

	void test_cxx_parser_finds_global_variable_function_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int one() { return 1; }\n"
			"int a = one();\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int a -> int one() <2:9 2:11>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> void App::operator+(int) <11:6 11:6>"
		));
	}

	void test_cxx_parser_finds_usage_of_function_pointer()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void my_int_func(int x)\n"
			"{\n"
			"}\n"
			"\n"
			"void test()\n"
			"{\n"
			"	void (*foo)(int);\n"
			"	foo = &my_int_func;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void test() -> void my_int_func(int) <8:9 8:19>"
		));
	}

	void test_cxx_parser_finds_usage_of_global_variable_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int bar;\n"
			"\n"
			"int main()\n"
			"{\n"
			"	bar = 1;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"int main() -> int bar <5:2 5:4>"
		));
	}

	void test_cxx_parser_finds_usage_of_global_variable_in_global_variable_initialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int a = 0;\n"
			"int b[] = {a};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"int [] b -> int a <2:12 2:12>"
		));
	}

	void test_cxx_parser_finds_usage_of_global_variable_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int bar;\n"
			"\n"
			"class App\n"
			"{\n"
			"	void foo()\n"
			"	{\n"
			"		bar = 1;\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void App::foo() -> int bar <7:3 7:5>"
		));
	}

	void test_cxx_parser_finds_usage_of_field_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"	void foo()\n"
			"	{\n"
			"		bar = 1;\n"
			"		this->bar = 2;\n"
			"	}\n"
			"	int bar;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void App::foo() -> int App::bar <5:3 5:5>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void App::foo() -> int App::bar <6:9 6:11>"
		));
	}

	void test_cxx_parser_finds_usage_of_field_in_initialization_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"	App()\n"
			"		: bar(42)\n"
			"	{}\n"
			"	int bar;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void App::App() -> int App::bar <4:5 4:7>"
		));
	}

	void test_cxx_parser_finds_usage_of_member_in_call_expression_to_unresolved_member_expression()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A {"
			"	template <typename T>\n"
			"	T run() { return 5; }\n"
			"};\n"
			"class B {\n"
			"	template <typename T>\n"
			"	T run() {\n"
			"		return a.run<T>();\n"
			"	}\n"
			"	A a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"T B::run<typename T>() -> A B::a <7:10 7:10>"
		));
	}

	void test_cxx_parser_finds_usage_of_member_in_temporary_object_expression()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Foo\n"
			"{\n"
			"public:\n"
			"	Foo() { }\n"
			"	Foo(const Foo& i, int d) { }\n"
			"};\n"
			"\n"
			"class Bar\n"
			"{\n"
			"public:\n"
			"	Bar(): m_i() {}\n"
			"\n"
			"	void baba()\n"
			"	{\n"
			"		Foo(m_i, 4);\n"
			"	}\n"
			"\n"
			"	const Foo m_i;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void Bar::baba() -> const Foo Bar::m_i <15:7 15:9>"
		));
	}

	void test_cxx_parser_finds_usage_of_member_in_dependent_scope_member_expression()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T m_t;\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		m_t.run();\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A<typename T>::foo() -> A<typename T>::T A<typename T>::m_t <8:3 8:5>"
		));
	}

	void test_cxx_parser_finds_return_type_use_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"double PI()\n"
			"{\n"
			"	return 3.14159265359;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"double PI() -> double <1:1 1:6>"
		));
	}

	void test_cxx_parser_finds_parameter_type_uses_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void ceil(float a)\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void ceil(float) -> float <1:11 1:15>"
		));
	}

	void test_cxx_parser_finds_use_of_decayed_parameter_type_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template<class T, unsigned int N>\n"
			"class VectorBase\n"
			"{\n"
			"public:\n"
			"	VectorBase(T values[N]);\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void VectorBase<class T, unsigned int N>::VectorBase<T, N>(VectorBase<class T, unsigned int N>::T []) -> VectorBase<class T, unsigned int N>::T <5:13 5:13>"
		));
	}

	void test_cxx_parser_usage_of_injected_type_in_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class Foo\n"
			"{\n"
			"	Foo& operator=(const Foo&) = delete;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"Foo<T> & Foo<typename T>::operator=(const Foo<T> &) -> Foo<T> <4:2 4:4>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"Foo<T> & Foo<typename T>::operator=(const Foo<T> &) -> Foo<T> <4:23 4:25>"
		));
	}

	void test_cxx_parser_finds_use_of_qualified_type_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void test(const int t)\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void test(const int) -> int <1:17 1:19>"
		));
	}

	void test_cxx_parser_finds_parameter_type_uses_in_constructor()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	A(int a);\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A::A(int) -> int <3:4 3:6>"
		));
	}

	void test_cxx_parser_finds_type_uses_in_function_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int main()\n"
			"{\n"
			"	int a = 42;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> int <3:2 3:4>"
		));
	}

	void test_cxx_parser_finds_type_uses_in_method_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	int main()\n"
			"	{\n"
			"		int a = 42;\n"
			"		return a;\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int A::main() -> int <5:3 5:5>"
		));
	}

	void test_cxx_parser_finds_type_uses_in_loops_and_conditions()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int main()\n"
			"{\n"
			"	if (true)\n"
			"	{\n"
			"		int a = 42;\n"
			"	}\n"
			"	for (int i = 0; i < 10; i++)\n"
			"	{\n"
			"		int b = i * 2;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> int <5:3 5:5>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> int <7:7 7:9>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> int <9:3 9:5>"
		));
	}

	void test_cxx_parser_finds_type_uses_of_base_class_in_derived_constructor()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"public:\n"
			"	A(int n) {}\n"
			"};\n"
			"class B : public A\n"
			"{\n"
			"public:\n"
			"	B() : A(42) {}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void B::B() -> A <9:8 9:8>"
		));
	}

	void test_cxx_parser_finds_enum_uses_in_global_space()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum A\n"
			"{\n"
			"	B,\n"
			"	C\n"
			"};\n"
			"A a = B;\n"
			"A* aPtr = new A;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"A a -> A::B <6:7 6:7>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A a -> A <6:1 6:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A * aPtr -> A <7:1 7:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A * aPtr -> A <7:15 7:15>"
		));
	}

	void test_cxx_parser_finds_enum_uses_in_function_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"enum A\n"
			"{\n"
			"	B,\n"
			"	C\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A a = B;\n"
			"	A* aPtr = new A;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"int main() -> A::B <8:8 8:8>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> A <8:2 8:2>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> A <9:2 9:2>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"int main() -> A <9:16 9:16>"
		));
	}

	void test_cxx_parser_finds_usage_of_template_parameter_of_template_member_variable_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"	struct IsBaseType {\n"
			"	static const bool value = true;\n"
			"};\n"
			"template <typename T>\n"
			"const bool IsBaseType<T>::value;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"IsBaseType<typename T>::T <1:20 1:20>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateParameterTypes, L"IsBaseType<typename T>::T <5:20 5:20>"
		));
	}

	void test_cxx_parser_finds_usage_of_template_parameters_with_different_depth_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	template <typename Q>\n"
			"	void foo(Q q)\n"
			"	{\n"
			"		T t;\n"
			"		t.run(q);\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A<typename T>::foo<typename Q>(Q) -> A<typename T>::T <7:3 7:3>"
			));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A<typename T>::foo<typename Q>(Q) -> A<typename T>::foo<typename Q>::Q <5:11 5:11>"
			));
	}

	void test_cxx_parser_finds_usage_of_template_parameters_with_different_depth_of_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	template <typename Q, typename R>\n"
			"	class B\n"
			"	{\n"
			"		T foo(Q q, R r);\n"
			"	};\n"
			"\n"
			"	template <typename R>\n"
			"	class B<int, R>\n"
			"	{\n"
			"		T foo(R r);\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<typename T>::T A<typename T>::B<int, typename R>::foo(A<typename T>::B<int, typename R>::R) -> A<typename T>::T <13:3 13:3>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<typename T>::T A<typename T>::B<int, typename R>::foo(A<typename T>::B<int, typename R>::R) -> A<typename T>::B<int, typename R>::R <13:9 13:9>"
		));
	}

	void test_cxx_parser_finds_usage_of_template_template_parameter_of_template_class_explicitly_instantiated_with_concrete_type_argument()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{\n"
			"	void foo(T<int> parameter)\n"
			"	{}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void B<template<typename> typename T>::foo(B<template<typename> typename T>::T<int>) -> B<template<typename> typename T>::T<int> <7:11 7:11>"
		));
	}

	void test_cxx_parser_finds_usage_of_template_template_parameter_of_template_class_explicitly_instantiated_with_template_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{\n"
			"	template <typename U> \n"
			"	void foo(T<U> parameter)\n"
			"	{}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void B<template<typename> typename T>::foo<typename U>(B<template<typename> typename T>::T<U>) -> B<template<typename> typename T>::T<B<template<typename> typename T>::foo<typename U>::U> <8:11 8:11>"
		));
	}

	void test_cxx_parser_finds_typedef_in_other_class_that_depends_on_own_template_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	typedef T type;\n"
			"};\n"
			"template <typename U>\n"
			"class B\n"
			"{\n"
			"public:\n"
			"	typedef typename A<U>::type type;\n"
			"};\n"
			"B<int>::type f = 0;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"B<typename U>::type -> A<B<typename U>::U>::type <11:25 11:28>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"B<int>::type -> A<int>::type <11:25 11:28>"
		));

		//TS_ASSERT_EQUALS(client->typeUses[3], L"A<int>::type -> int <13:9 13:12>"); TODO: make this work!
	}

	void test_cxx_parser_finds_use_of_dependent_template_specialization_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	template <typename X>\n"
			"	using type = T;\n"
			"};\n"
			"template <typename U>\n"
			"class B\n"
			"{\n"
			"public:\n"
			"	typedef typename A<U>::template type<float> type;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"B<typename U>::type -> A<B<typename U>::U>::type<float> <12:10 12:17>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<int> -> int <7:4 7:6>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_of_explicit_template_instantiated_with_function_prototype()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"void foo()\n"
			"{\n"
			"	A<int()> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<int()> -> int <7:4 7:6>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_for_parameter_pack_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename... T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"   A<int, float>();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<<int, float>> -> int <7:6 7:8>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<<int, float>> -> float <7:11 7:15>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_in_non_default_constructor_of_explicit_template_instaitiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	A(int data){}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int>(5);\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<int> -> int <9:4 9:6>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_in_default_constructor_of_explicit_template_instaitiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	A(){}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int>();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<int> -> int <9:4 9:6>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_in_new_expression_of_explicit_template_instaitiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	A(){}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	new A<int>();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<int> -> int <9:8 9:10>"
		));
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<1> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_bool_template_parameter_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<true> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_argument_of_implicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"int main()\n"
			"{\n"
			"	A<&g_p> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p> -> P g_p <9:5 9:7>"
		));
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_argument_of_implicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"int main()\n"
			"{\n"
			"	A<g_p> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p> -> P g_p <9:4 9:6>"
		));
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_pack_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int... T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"   A<1, 2, 33>();\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_template_template_argument_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{};\n"
			"int main()\n"
			"{\n"
			"	B<A> ba;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"B<A> -> A<typename T> <9:4 9:4>"
		));
	}

	void test_cxx_parser_finds_template_template_argument_for_parameter_pack_of_explicit_template_instantiation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <template<typename> typename... T>\n"
			"class B\n"
			"{\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	B<A, A>();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"B<<A, A>> -> A<typename T> <11:4 11:4>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"B<<A, A>> -> A<typename T> <11:7 11:7>"
		));
	}

	void test_cxx_parser_finds_template_member_specialization_for_method_of_implicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	T foo() {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateMemberSpecializations, L"int A<int>::foo() -> A<typename T>::T A<typename T>::foo() <5:4 5:6>"
		));
	}

	void test_cxx_parser_finds_template_member_specialization_for_static_variable_of_implicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	static T foo;\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateMemberSpecializations, L"static int A<int>::foo -> static A<typename T>::T A<typename T>::foo <5:11 5:13>"
		));
	}

	void test_cxx_parser_finds_template_member_specialization_for_field_of_implicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	T foo;\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateMemberSpecializations, L"int A<int>::foo -> A<typename T>::T A<typename T>::foo <5:4 5:6>"
		));
	}

	void test_cxx_parser_finds_template_member_specialization_for_field_of_member_class_of_implicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	class B {\n"
			"	public:\n"
			"		T foo;\n"
			"	};\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int>::B b;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateMemberSpecializations, L"int A<int>::B::foo -> A<typename T>::T A<typename T>::B::foo <7:5 7:7>"
		));
	}

	void test_cxx_parser_finds_template_member_specialization_for_member_class_of_implicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"public:\n"
			"	class B {};\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	A<int> a;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateMemberSpecializations, L"A<int>::B -> A<typename T>::B <5:8 5:8>"
		));
	}

	void test_cxx_parser_finds_type_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<int> -> int <6:9 6:11>"
		));
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<1>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_bool_template_parameter_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <>\n"
			"class A<true>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_non_type_custom_pointer_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <>\n"
			"class A<&g_p>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p> -> P g_p <8:10 8:12>"
		));
	}

	void test_cxx_parser_finds_non_type_custom_reference_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <>\n"
			"class A<g_p>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p> -> P g_p <8:9 8:11>"
		));
	}

	void test_cxx_parser_finds_template_template_argument_of_explicit_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T>\n"
			"class B\n"
			"{};\n"
			"template <>\n"
			"class B<A>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"B<A> -> A<typename T> <8:9 8:9>"
		));
	}

	void test_cxx_parser_finds_type_template_arguments_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T, typename U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T>\n"
			"class A<T, int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<typename T, int> -> A<typename T, int>::T <6:9 6:9>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<typename T, int> -> int <6:12 6:14>"
		));
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_int_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T, int U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <int U>\n"
			"class A<3, U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<3, int U> -> A<3, int U>::U <6:12 6:12>"
		));
	}

	void test_cxx_parser_finds_no_template_argument_for_builtin_non_type_bool_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T, bool U>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <bool U>\n"
			"class A<true, U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<true, bool U> -> A<true, bool U>::U <6:15 6:15>"
		));
	}

	void test_cxx_parser_finds_template_argument_for_non_type_custom_pointer_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P* p, P* q>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <P* q>\n"
			"class A<&g_p, q>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p, P * q> -> P g_p <8:10 8:12>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p, P * q> -> A<&g_p, P * q>::q <8:15 8:15>"
		));
	}

	void test_cxx_parser_finds_template_argument_for_non_type_custom_reference_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class P\n"
			"{};\n"
			"template <P& p, P& q>\n"
			"class A\n"
			"{};\n"
			"P g_p;\n"
			"template <P& q>\n"
			"class A<g_p, q>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p, P & q> -> P g_p <8:9 8:11>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<&g_p, P & q> -> A<&g_p, P & q>::q <8:14 8:14>"
		));
	}

	void test_cxx_parser_finds_template_argument_for_template_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T, template<typename> class U>\n"
			"class B\n"
			"{};\n"
			"template <template<typename> class U>\n"
			"class B<A, U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"B<A, template<typename> typename U> -> A<typename T> <8:9 8:9>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"B<A, template<typename> typename U> -> B<A, template<typename> typename U>::U<typename> <8:12 8:12>"
		));
	}

	void test_cxx_parser_finds_non_type_template_argument_that_depends_on_type_template_parameter_of_explicit_partial_class_template_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T1, typename T2, T2 T3>\n"
			"class A\n"
			"{\n"
			"};\n"
			"template <typename T2, T2 T3>\n"
			"class A<3, T2, T3>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<3, typename T2, T2 T3> -> A<3, typename T2, T2 T3>::T3 <6:16 6:17>"
		));
	}

	//void _test_cxx_parser_finds_non_type_template_argument_that_depends_on_template_template_parameter_of_explicit_partial_class_template_specialization()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"template <int T1, template<typename> class T2, T2<int> T3>\n"
	//		"class A\n"
	//		"{\n"
	//		"};\n"
	//		"template <template<typename> class T2, T2<int> T3>\n"
	//		"class A<3, T2, T3>\n"
	//		"{\n"
	//		"};\n"
	//	);

	//	TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 2);
	//	TS_ASSERT_EQUALS(client->templateArgumentTypes[0], L"A<3, template<typename> typename T2, T2<int> T3> -> A<3, template<typename> typename T2, T2<int> T3>::T2<typename> <6:12 6:13>");
	//	TS_ASSERT_EQUALS(client->templateArgumentTypes[1], L"A<3, template<typename> typename T2, T2<int> T3> -> A<3, template<typename> typename T2, T2<int> T3>::T3 <6:16 6:17>");
	//}

	void test_cxx_parser_finds_implicit_template_class_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
			"\n"
			"A<int> a;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateSpecializations, L"A<int> -> A<typename T> <2:7 2:7>"
		));
	}

	void test_cxx_parser_finds_class_inheritance_from_implicit_template_class_specialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
			"\n"
			"class B: public A<int>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"B -> A<int> <7:17 7:17>"
		));
	}

	void test_cxx_parser_finds_template_class_specialization_with_template_argument()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo;\n"
			"};\n"
			"\n"
			"template <typename U>\n"
			"class B: public A<U>\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"A<B<typename U>::U> -> B<typename U>::U <8:19 8:19>"
		));
	}

	void test_cxx_parser_finds_template_class_constructor_usage_of_field()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	A(): foo() {}\n"
			"	T foo;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A<typename T>::A<T>() -> A<typename T>::T A<typename T>::foo <4:7 4:9>"
		));
	}

	void test_cxx_parser_finds_correct_method_return_type_of_template_class_in_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{\n"
			"	T foo();\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<typename T>::T A<typename T>::foo() -> A<typename T>::T <4:2 4:2>"
		));
	}

	void test_cxx_parser_finds_type_template_default_argument_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T = int>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateDefaultArgumentTypes, L"A<typename T>::T -> int <1:24 1:26>"
		));
	}

	void test_cxx_parser_finds_no_default_argument_type_for_non_type_bool_template_parameter_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T = true>\n"
			"class A\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_template_template_default_argument_type_of_template_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T = A>\n"
			"class B\n"
			"{};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateDefaultArgumentTypes, L"B<template<typename> typename T>::T<typename> -> A<typename T> <4:40 4:40>"
		));
	}

	void test_cxx_parser_finds_implicit_instantiation_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	return test(1);\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateSpecializations, L"int test<int>(int) -> T test<typename T>(T) <2:3 2:6>"
		));
	}

	void test_cxx_parser_finds_explicit_specialization_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T a)\n"
			"{\n"
			"	return a;\n"
			"};\n"
			"\n"
			"template <>\n"
			"int test<int>(int a)\n"
			"{\n"
			"	return a + a;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateSpecializations, L"int test<int>(int) -> T test<typename T>(T) <8:5 8:8>"
		));
	}

	void test_cxx_parser_finds_explicit_type_template_argument_of_explicit_instantiation_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void test()\n"
			"{\n"
			"};\n"
			"\n"
			"template <>\n"
			"void test<int>()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"void test<int>() -> int <7:11 7:13>"
		));
	}

	void test_cxx_parser_finds_explicit_type_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void test(){}\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test<int>();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"void test<int>() -> int <6:7 6:9>"
		));
	}

	void test_cxx_parser_finds_no_explicit_non_type_int_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <int T>\n"
			"void test(){}\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test<33>();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_explicit_template_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A {};\n"
			"template <template<typename> class T>\n"
			"void test(){};\n"
			"int main()\n"
			"{\n"
			"	test<A>();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"void test<A>() -> A<typename T> <7:7 7:7>"
		));
	}

	void test_cxx_parser_finds_no_implicit_type_template_argument_of_function_call_in_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void test(T data){}\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test(1);\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_explicit_type_template_argument_of_function_call_in_var_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(){ return 1; }\n"
			"\n"
			"class A\n"
			"{\n"
			"	int foo = test<int>();\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"int test<int>() -> int <6:17 6:19>"
		));
	}

	void test_cxx_parser_finds_no_implicit_type_template_argument_of_function_call_in_var_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"T test(T i){ return i; }\n"
			"\n"
			"class A\n"
			"{\n"
			"	int foo = test(1);\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_type_template_default_argument_type_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T = int>\n"
			"void test()\n"
			"{\n"
			"};\n"
			"\n"
			"int main()\n"
			"{\n"
			"	test();\n"
			"	return 1;\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateDefaultArgumentTypes, L"test<typename T>::T -> int <1:24 1:26>"
		));
	}

	void test_cxx_parser_does_not_find_default_argument_type_for_non_type_bool_template_parameter_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <bool T = true>\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->templateDefaultArgumentTypes.size(), 0);
	}

	void test_cxx_parser_finds_template_template_default_argument_type_of_template_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class A\n"
			"{};\n"
			"template <template<typename> class T = A>\n"
			"void test()\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateDefaultArgumentTypes, L"test<template<typename> typename T>::T<typename> -> A<typename T> <4:40 4:40>"
		));
	}

	void test_cxx_parser_finds_lambda_calling_a_function()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void func() {}\n"
			"void lambdaCaller()\n"
			"{\n"
			"	[]()\n"
			"	{\n"
			"		func();\n"
			"	}();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void lambdaCaller::lambda at 4:2() const -> void func() <6:3 6:6>"
		));
	}

	void test_cxx_parser_finds_local_variable_in_lambda_capture()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void lambdaWrapper()\n"
			"{\n"
			"	int x = 2;\n"
			"	[x]() { return 1; }();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:6> <4:3 4:3>"
		));
	}

	void test_cxx_parser_finds_usage_of_local_variable_in_microsoft_inline_assembly_statement()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void foo()\n"
			"{\n"
			"	int x = 2;\n"
				"__asm\n"
				"{\n"
				"	mov eax, x\n"
				"	mov x, eax\n"
				"}\n"
			"}\n",
			{ L"--target=i686-pc-windows-msvc" }
		);

 		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:6> <6:11 6:11>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:6> <7:6 7:6>"
		));
	}

	void test_cxx_parser_finds_template_argument_of_unresolved_lookup_expression()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"void a()\n"
			"{\n"
			"}\n"
			"\n"
			"template<typename MessageType>\n"
			"void dispatch()\n"
			"{\n"
			"	a<MessageType>();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->templateArgumentTypes, L"void dispatch<typename MessageType>() -> dispatch<typename MessageType>::MessageType <9:4 9:14>"
		));
	}

	///////////////////////////////////////////////////////////////////////////////
	// test finding symbol locations

	void test_cxx_parser_finds_correct_location_of_explicit_constructor_defined_in_namespace()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n\n"
			"{\n"
			"	class App\n"
			"	{\n"
			"	public:\n"
			"		App(int i) {}\n"
			"	};\n"
			"}\n"
			"int main()\n"
			"{\n"
			"	n::App a = n::App(2);\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int main() -> void n::App::App(int) <11:16 11:18>"
		));
	}

	void test_cxx_parser_finds_macro_argument_location_for_field_definition_with_name_passed_as_argument_to_macro()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define DEF_INT_FIELD(name) int name;\n"
			"class A {\n"
			"	DEF_INT_FIELD(m_value)\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private int A::m_value <3:16 3:22>"
		));
	}

	void test_cxx_parser_finds_macro_usage_location_for_field_definition_with_name_partially_passed_as_argument_to_macro()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define DEF_INT_FIELD(name) int m_##name;\n"
			"class A {\n"
			"	DEF_INT_FIELD(value)\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private int A::m_value <3:2 3:14>"
		));
	}


	void test_cxx_parser_finds_macro_argument_location_for_function_call_in_code_passed_as_argument_to_macro()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define DEF_INT_FIELD(name, init) int name = init;\n"
			"int foo() { return 5; }\n"
			"class A {\n"
			"	DEF_INT_FIELD(m_value, foo())\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int A::m_value -> int foo() <4:25 4:27>"
		));
	}


	void test_cxx_parser_finds_macro_usage_location_for_function_call_in_code_of_macro_body()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int foo() { return 5; }\n"
			"#define DEF_INT_FIELD(name) int name = foo();\n"
			"class A {\n"
			"	DEF_INT_FIELD(m_value)\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"int A::m_value -> int foo() <4:2 4:14>"
		));
	}


	//void __test_cxx_parser_finds_type_template_argument_of_static_cast_expression()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"int main()\n"
	//		"{\n"
	//		"	return static_cast<int>(4.0f);"
	//		"}\n"
	//	);

	//	TS_ASSERT_EQUALS(client->templateArgumentTypes.size(), 1);
	//	TS_ASSERT_EQUALS(client->templateArgumentTypes[0], L"A<1> -> int <0:0 0:0>");
	//}

	//void _test_cxx_parser_finds_implicit_constructor_call_in_initialization()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"class A\n"
	//		"{\n"
	//		"};\n"
	//		"class B\n"
	//		"{\n"
	//		"	B(){}\n"
	//		"	A m_a;\n"
	//		"};\n"
	//	);

	//	TS_ASSERT_EQUALS(client->calls.size(), 1);
	//	TS_ASSERT_EQUALS(client->calls[0], L"void B::B() -> A::A() <6:2 6:2>");
	//}

	void test_cxx_parser_parses_multiple_files()
	{
		const std::set<FilePath> indexedPaths = { FilePath(L"data/CxxParserTestSuite/") };
		const std::set<FilePathFilter> excludeFilters;
		const std::set<FilePathFilter> includeFilters;
		const FilePath workingDirectory(L".");
		const FilePath sourceFilePath(L"data/CxxParserTestSuite/code.cpp");

		std::shared_ptr<IndexerCommandCxx> indexerCommand = std::make_shared<IndexerCommandCxx>(
			sourceFilePath,
			indexedPaths,
			excludeFilters,
			includeFilters,
			workingDirectory,
			std::vector<FilePath>(),
			std::vector<FilePath>(),
			std::vector<std::wstring> { L"--target=x86_64-pc-windows-msvc", L"-std=c++1z", sourceFilePath.wstr() }
		);

		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();
		CxxParser parser(client, std::make_shared<TestFileRegister>());

		parser.buildIndex(indexerCommand);

		TS_ASSERT_EQUALS(client->errors.size(), 0);

		TS_ASSERT_EQUALS(client->typedefs.size(), 1);
		TS_ASSERT_EQUALS(client->classes.size(), 5);
		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enumConstants.size(), 2);
		TS_ASSERT_EQUALS(client->functions.size(), 5); // used methods are also recorded as functions (these get overridden in the intermediate storage)
		TS_ASSERT_EQUALS(client->fields.size(), 4);
		TS_ASSERT_EQUALS(client->globalVariables.size(), 2);
		TS_ASSERT_EQUALS(client->methods.size(), 15);
		TS_ASSERT_EQUALS(client->namespaces.size(), 2);
		TS_ASSERT_EQUALS(client->structs.size(), 1);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->calls.size(), 3);
		TS_ASSERT_EQUALS(client->usages.size(), 3);
		TS_ASSERT_EQUALS(client->typeUses.size(), 16);

		TS_ASSERT_EQUALS(client->files.size(), 2);
		TS_ASSERT_EQUALS(client->includes.size(), 1);
	}


	void test_cxx_parser_finds_braces_of_class_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:1> <2:1 2:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:1> <3:1 3:1>"
		));
	}

	void test_cxx_parser_finds_braces_of_namespace_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"namespace n\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:1> <2:1 2:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:1> <3:1 3:1>"
		));
	}

	void test_cxx_parser_finds_braces_of_function_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int main()\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:1> <2:1 2:1>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:1> <3:1 3:1>"
		));
	}

	void test_cxx_parser_finds_braces_of_method_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class App\n"
			"{\n"
			"public:\n"
			"	App(int i) {}\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:13> <4:13 4:13>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:13> <4:14 4:14>"
		));
	}

	void test_cxx_parser_finds_braces_of_init_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int a = 0;\n"
			"int b[] = {a};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:11> <2:11 2:11>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<2:11> <2:13 2:13>"
		));
	}

	void test_cxx_parser_finds_braces_of_lambda()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void lambdaCaller()\n"
			"{\n"
			"	[](){}();\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:6> <3:6 3:6>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:6> <3:7 3:7>"
		));
	}

	void test_cxx_parser_finds_braces_of_asm_stmt()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void foo()\n"
			"{\n"
			"	__asm\n"
			"	{\n"
			"		mov eax, eax\n"
			"	}\n"
			"}\n",
			{ L"--target=i686-pc-windows-msvc" }
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:2> <4:2 4:2>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:2> <6:2 6:2>"
		));
	}

	void test_cxx_parser_finds_no_duplicate_braces_of_template_class_and_method_decl()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"template <typename T>\n"
			"class App\n"
			"{\n"
			"public:\n"
			"	App(int i) {}\n"
			"};\n"
			"int main()\n"
			"{\n"
			"	App<int> a;\n"
			"	return 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 9);
	}

	void test_cxx_parser_finds_correct_signature_location_of_constructor_with_initializer_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class A\n"
			"{\n"
			"	A(const int& foo) : m_foo(foo)\n"
			"	{\n"
			"	}\n"
			"	const int m_foo\n"
			"}\n"
		);;

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"private void A::A(const int &) <3:2 <3:2  <3:2 3:2> 3:18> 5:2>"
		));
	}

	void test_cxx_parser_catches_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"int a = b;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->errors, L"use of undeclared identifier \'b\' <1:9 1:9>"
		));
	}

	void test_cxx_parser_catches_error_in_force_include()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"void foo() {} \n", { L"-include nothing" }
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->errors, L"' nothing' file not found <1:1 1:1>"
		));
	}

	void test_cxx_parser_finds_correct_error_location_after_line_directive()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#line 55 \"foo.hpp\"\n"
			"void foo()\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->errors, L"expected function body after function declarator <2:11 2:11>"
		));
	}

	void test_cxx_parser_catches_error_in_macro_expansion()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"#define MACRO_WITH_NONEXISTING_PATH \"this_path_does_not_exist.txt\"\n"
			"#include MACRO_WITH_NONEXISTING_PATH\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->errors, L"'this_path_does_not_exist.txt' file not found <2:10 2:10>"
		));
	}

	void test_cxx_parser_finds_location_of_line_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"// this is a line comment\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->comments, L"comment <1:1 1:26>"
		));
	}

	void test_cxx_parser_finds_location_of_block_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"/* this is a\n"
			"block comment */\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->comments, L"comment <1:1 2:17>"
		));
	}

	// void _test_TEST()
	// {
	// 	std::shared_ptr<TestParserClient> client = parseCode(
	// 		"template <template<template<typename> class> class T>\n"
	// 		"class A {\n"
	// 		"T<>\n"
	// 		"};\n"
	// 		"template <template<typename> class T>\n"
	// 		"class B {};\n"
	// 		"template <typename T>\n"
	// 		"class C {};\n"
	// 		"A<B> a;\n"
	// 	);
	// 	int ofo = 0;
	// }

private:
	std::shared_ptr<TestParserClient> parseCode(std::string code, std::vector<std::wstring> compilerFlags = {})
	{
		std::shared_ptr<TestFileRegister> fileRegister = std::make_shared<TestFileRegister>();
		std::shared_ptr<TestParserClient> parserClient = std::make_shared<TestParserClient>();
		CxxParser parser(parserClient, fileRegister);
		parser.buildIndex(L"input.cc", TextAccess::createFromString(code), utility::concat(compilerFlags, std::vector<std::wstring>(1, L"-std=c++1z")));
		return parserClient;
	}
};
