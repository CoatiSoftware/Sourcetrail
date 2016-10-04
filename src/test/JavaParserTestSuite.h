#include "cxxtest/TestSuite.h"


#include "utility/file/FileRegister.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaParser.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

#include "helper/TestFileManager.h"



class JavaParserTestSuite: public CxxTest::TestSuite
{
public:
///////////////////////////////////////////////////////////////////////////////
// test finding symbol definitions and declarations

	void test_java_parser_can_setup_environment_factory()
	{
		setupJavaEnvironmentFactory();

		// if this one fails, maybe your java_path in the test settings is wrong.
		TS_ASSERT_LESS_THAN_EQUALS(1, JavaEnvironmentFactory::getInstance().use_count());
	}

	void test_java_parser_finds_package_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
		);

		TS_ASSERT_EQUALS(client->packages.size(), 1);
		TS_ASSERT_EQUALS(client->packages[0], "foo <1:1 <1:9 1:11> 1:12>");
	}

	void test_java_parser_finds_class_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "public A <1:1 <1:14 1:14> 3:1>");
	}

	void test_java_parser_finds_interface_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public interface A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->interfaces.size(), 1);
		TS_ASSERT_EQUALS(client->interfaces[0], "public A <1:1 <1:18 1:18> 3:1>");
	}

	void test_java_parser_finds_class_declaration_in_named_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "public foo.A <2:1 <2:14 2:14> 4:1>");
	}

	void test_java_parser_finds_class_declaration_in_nested_named_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo.bar;\n"
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 1);
		TS_ASSERT_EQUALS(client->classes[0], "public foo.bar.A <2:1 <2:14 2:14> 4:1>");
	}

	void test_java_parser_finds_enum_declaration_in_named_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public enum A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->enums.size(), 1);
		TS_ASSERT_EQUALS(client->enums[0], "public foo.A <2:1 <2:13 2:13> 4:1>");
	}

	void test_java_parser_finds_enum_constant_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public enum A\n"
			"{\n"
			"	A_TEST(0)\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->enumConstants.size(), 1);
		TS_ASSERT_EQUALS(client->enumConstants[0], "foo.A.A_TEST <4:2 4:10>");
	}

	void test_java_parser_finds_constructor_declaration_without_parameters()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	public A()\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "public foo.A.A() <4:2 <4:9 4:9> 6:2>");
	}

	void test_java_parser_finds_method_declaration_with_custom_type_in_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	public void bar(A a)\n"
			"	{\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->methods.size(), 1);
		TS_ASSERT_EQUALS(client->methods[0], "public void foo.A.bar(foo.A) <4:2 <4:14 4:16> 6:2>");
	}

	void test_java_parser_finds_field_declaration_with_initial_assignment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	int bar = 0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "default foo.A.bar <4:6 4:8>");
	}

	void test_java_parser_finds_public_access_specifier_in_field_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	public int bar;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "public foo.A.bar <4:13 4:15>");
	}

	void test_java_parser_finds_protected_access_specifier_in_field_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	protected int bar;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "protected foo.A.bar <4:16 4:18>");
	}

	void test_java_parser_finds_private_access_specifier_in_field_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	private int bar;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->fields.size(), 1);
		TS_ASSERT_EQUALS(client->fields[0], "private foo.A.bar <4:14 4:16>");
	}

	void test_java_parser_finds_definition_of_method_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	void bar(int i)\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 5);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<3:1> <3:1 3:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<3:1> <7:1 7:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<4:15> <4:15 4:15>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<5:2> <5:2 5:2>");
		TS_ASSERT_EQUALS(client->localSymbols[4], "input.cc<5:2> <6:2 6:2>");
	}

	void test_java_parser_finds_definition_of_local_variable()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	void bar()\n"
			"	{\n"
			"		int i;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 5);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<3:1> <3:1 3:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<3:1> <8:1 8:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<5:2> <5:2 5:2>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<5:2> <7:2 7:2>");
		TS_ASSERT_EQUALS(client->localSymbols[4], "input.cc<6:7> <6:7 6:7>");
	}

	void test_java_parser_finds_type_argument_name_in_signature_of_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"	public A<Void> foo(A<Void> a){\n"
			"		return a;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeParameters.size(), 1);
		TS_ASSERT_EQUALS(client->typeParameters[0], "A<T>.T <1:17 1:17>");
	}

	void test_java_parser_finds_definition_of_type_parameter_of_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeParameters.size(), 1);
		TS_ASSERT_EQUALS(client->typeParameters[0], "A<T>.T <1:17 1:17>");
	}

	void test_java_parser_finds_definition_of_type_parameter_of_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public <T> void foo()\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeParameters.size(), 1);
		TS_ASSERT_EQUALS(client->typeParameters[0], "A.foo<T>.T <3:10 3:10>");
	}





	void test_java_parser_finds_line_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"// this is a line comment\n"
			"package foo;\n"
		);

		TS_ASSERT_EQUALS(client->comments.size(), 1);
		TS_ASSERT_EQUALS(client->comments[0], "comment <1:1 1:26>");
	}

	void test_java_parser_finds_block_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"/* this is a line comment*/\n"
			"package foo;\n"
		);

		TS_ASSERT_EQUALS(client->comments.size(), 1);
		TS_ASSERT_EQUALS(client->comments[0], "comment <1:1 1:28>");
	}

	void test_java_parser_finds_missing_semicolon_as_parse_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo\n"
		);

		TS_ASSERT_EQUALS(client->errors.size(), 1);
		TS_ASSERT_EQUALS(client->errors[0], "Encountered unexpected token. <1:1 1:1>");
	}

	//void _test_java_parser_finds_missing_import_as_error()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"import foo;\n"
	//	);

	//	TS_ASSERT_EQUALS(client->errors.size(), 1);
	//	TS_ASSERT_EQUALS(client->errors[0], "Import not found. <1:8 1:10>");
	//}




///////////////////////////////////////////////////////////////////////////////
// test finding nested symbol definitions and declarations

	void test_java_parser_finds_class_declaration_nested_in_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo.bar;\n"
			"public class A\n"
			"{\n"
			"	public class B\n"
			"	{\n"
			"	}\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "public foo.bar.A <2:1 <2:14 2:14> 7:1>");
		TS_ASSERT_EQUALS(client->classes[1], "public foo.bar.A.B <4:2 <4:15 4:15> 6:2>");
	}

	void test_java_parser_finds_class_declaration_nested_in_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo.bar;\n"
			"public class A\n"
			"{\n"
			"	public void bar(int i)\n"
			"	{\n"
			"		class B\n"
			"		{\n"
			"		}\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT_EQUALS(client->classes.size(), 2);
		TS_ASSERT_EQUALS(client->classes[0], "public foo.bar.A <2:1 <2:14 2:14> 10:1>");
		TS_ASSERT_EQUALS(client->classes[1], "default foo.bar.A.bar.B <6:3 <6:9 6:9> 8:3>");
	}


///////////////////////////////////////////////////////////////////////////////
// test finding usages of symbols

	void test_java_parser_finds_name_of_package_imported_with_asterisk()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"import foo.bar.*;\n"
		);

		TS_ASSERT_EQUALS(client->imports.size(), 1);
		TS_ASSERT_EQUALS(client->imports[0], "input.cc -> foo.bar <1:8 1:14>");
	}

	void test_java_parser_finds_inheritance_using_extends_keyword()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"}\n"
			"\n"
			"public class B extends A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "foo.B -> foo.A <6:24 6:24>");
	}

	void test_java_parser_finds_inheritance_using_implements_keyword()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"}\n"
			"\n"
			"public class B implements A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->inheritances.size(), 1);
		TS_ASSERT_EQUALS(client->inheritances[0], "foo.B -> foo.A <6:27 6:27>");
	}

	void test_java_parser_finds_method_override()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	void bar()\n"
			"	{}\n"
			"}\n"
			"public class B extends A\n"
			"{\n"
			"	void bar()\n"
			"	{}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->overrides.size(), 1);
		TS_ASSERT_EQUALS(client->overrides[0], "void foo.B.bar() -> void foo.A.bar() <9:7 9:9>");
	}

	void test_java_parser_finds_method_call()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	void bar()\n"
			"	{\n"
			"		bar();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "void foo.A.bar() -> void foo.A.bar() <6:3 6:5>");
	}

	void _SYMBOLSOLVERISSUE_test_java_parser_finds_correct_location_of_qualified_type_usage()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class Foo {\n"
			"	public class Base {\n"
			"		public class X {\n"
			"		}\n"
			"	}\n"
			"	public class Derived extends Base {\n"
			"		public X x = null;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 3);
		TS_ASSERT_EQUALS(client->typeUses[0], "Derived.x -> Base.X ###");
	}

	void test_java_parser_finds_correct_location_of_qualified_type_usage()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public class B\n"
			"	{\n"
			"	}\n"
			"	void bar()\n"
			"	{\n"
			"		B b = new A.B();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 3);
		TS_ASSERT_EQUALS(client->typeUses[0], "void A.bar() -> void <6:2 6:5>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void A.bar() -> A.B <8:3 8:3>");
		TS_ASSERT_EQUALS(client->typeUses[2], "void A.bar() -> A.B <8:13 8:15>");
	}

	void ___test_java_parser_finds_method_call_to_super()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	public class A\n"
			"	{\n"
			"		void bar()\n"
			"		{\n"
			"		}\n"
			"	}\n"
			"	\n"
			"	public class B extends A\n"
			"	{\n"
			"		void bar()\n"
			"		{\n"
			"			super.bar();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
		TS_ASSERT_EQUALS(client->calls[0], "void foo.A.bar() -> void foo.A.bar() <6:3 6:5>");
	}

	void ___test_java_parser_finds_usage_of_field_with_same_name_as_method_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	private int t;\n"
			"	public X(int t)\n"
			"	{\n"
			"		this.t = t;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
//		TS_ASSERT_EQUALS(client->calls[0], "void foo.A.bar() -> void foo.A.bar() <6:3 6:5>");
	}

	void ___test_java_parser_does_not_confuse_method_name_with_field_name()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	private int foo;\n"
			"	public foo()\n"
			"	{\n"
			"		this.foo = 5;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->calls.size(), 1);
	//	TS_ASSERT_EQUALS(client->calls[0], "void foo.A.bar() -> void foo.A.bar() <6:3 6:5>");
	}

	void test_java_parser_finds_assignment_of_method_argument()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	void bar(int i)\n"
			"	{\n"
			"		i = 0;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 6);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<3:1> <3:1 3:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<3:1> <8:1 8:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<4:15> <4:15 4:15>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<5:2> <5:2 5:2>");
		TS_ASSERT_EQUALS(client->localSymbols[4], "input.cc<5:2> <7:2 7:2>");
		TS_ASSERT_EQUALS(client->localSymbols[5], "input.cc<4:15> <6:3 6:3>");
	}

	void test_java_parser_finds_assignment_of_local_variable()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	void bar()\n"
			"	{\n"
			"		int i;\n"
			"		i = 0;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 6);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<3:1> <3:1 3:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<3:1> <9:1 9:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<5:2> <5:2 5:2>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<5:2> <8:2 8:2>");
		TS_ASSERT_EQUALS(client->localSymbols[4], "input.cc<6:7> <6:7 6:7>");
		TS_ASSERT_EQUALS(client->localSymbols[5], "input.cc<6:7> <7:3 7:3>");
	}

	void test_java_parser_finds_scope_of_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 2);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <3:1 3:1>");
	}

	void test_java_parser_finds_scope_of_enum_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public enum A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 2);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <3:1 3:1>");
	}

	void test_java_parser_finds_scope_of_constructor_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public A()\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 4);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <6:1 6:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<4:2> <4:2 4:2>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<4:2> <5:2 5:2>");
	}

	void test_java_parser_finds_scope_of_method_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public void a()\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 4);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <6:1 6:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<4:2> <4:2 4:2>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<4:2> <5:2 5:2>");
	}

	void test_java_parser_finds_scope_of_switch_statement()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public void a()\n"
			"	{\n"
			"		switch(2)\n"
			"		{\n"
			"		case 1:\n"
			"			break;\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 6);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <11:1 11:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<4:2> <4:2 4:2>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<4:2> <10:2 10:2>");
		TS_ASSERT_EQUALS(client->localSymbols[4], "input.cc<6:3> <6:3 6:3>");
		TS_ASSERT_EQUALS(client->localSymbols[5], "input.cc<6:3> <9:3 9:3>");
	}

	void test_java_parser_finds_scope_of_block_statement()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public void a()\n"
			"	{\n"
			"		{\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 6);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <8:1 8:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<4:2> <4:2 4:2>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<4:2> <7:2 7:2>");
		TS_ASSERT_EQUALS(client->localSymbols[4], "input.cc<5:3> <5:3 5:3>");
		TS_ASSERT_EQUALS(client->localSymbols[5], "input.cc<5:3> <6:3 6:3>");
	}

	void test_java_parser_finds_scope_of_array_initialization_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	private int[] array = {1, 2};\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->localSymbols.size(), 4);
		TS_ASSERT_EQUALS(client->localSymbols[0], "input.cc<2:1> <2:1 2:1>");
		TS_ASSERT_EQUALS(client->localSymbols[1], "input.cc<2:1> <4:1 4:1>");
		TS_ASSERT_EQUALS(client->localSymbols[2], "input.cc<3:24> <3:24 3:24>");
		TS_ASSERT_EQUALS(client->localSymbols[3], "input.cc<3:24> <3:29 3:29>");
	}

	void test_java_parser_finds_usage_of_type_parameter_of_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"	T t;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<T>.t -> A<T>.T <3:2 3:2>");
	}

	void test_java_parser_finds_usage_of_type_parameter_of_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public <T> void foo(T t){};\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 2);
		TS_ASSERT_EQUALS(client->typeUses[0], "void A.foo<T>(T) -> void <3:13 3:16>");
		TS_ASSERT_EQUALS(client->typeUses[1], "void A.foo<T>(T) -> A.foo<T>.T <3:22 3:22>");
	}

	void test_java_parser_finds_correct_location_of_generic_type_usage()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"	A<Void> t;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<T>.t -> A<T> <3:2 3:2>");
	}

	void test_java_parser_finds_bound_type_of_type_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T extends Void>\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->typeUses.size(), 1);
		TS_ASSERT_EQUALS(client->typeUses[0], "A<T>.T -> java.lang.Void <1:27 1:30>");
	}


	//void test_cxx_parser_catches_error()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"int a = b;\n",
	//		false
	//	);

	//	TS_ASSERT_EQUALS(client->errors.size(), 1);
	//	TS_ASSERT_EQUALS(client->errors[0], "use of undeclared identifier \'b\' <1:9 1:9>");
	//}

	//void test_cxx_parser_finds_location_of_line_comment()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"// this is a line comment\n"
	//	);

	//	TS_ASSERT_EQUALS(client->comments.size(), 1);
	//	TS_ASSERT_EQUALS(client->comments[0], "comment <1:1 1:26>");
	//}

	//void test_cxx_parser_finds_location_of_block_comment()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"/* this is a\n"
	//		"block comment */\n"
	//	);

	//	TS_ASSERT_EQUALS(client->comments.size(), 1);
	//	TS_ASSERT_EQUALS(client->comments[0], "comment <1:1 2:17>");
	//}


private:
	class TestParserClient: public ParserClient
	{
	public:
		virtual void startParsingFile()
		{
		}

		virtual void finishParsingFile()
		{
		}


		std::vector<std::string>* getBinForSymbolKind(SymbolKind symbolType)
		{
			switch (symbolType)
			{
			case SYMBOL_PACKAGE:
				return &packages;
			case SYMBOL_CLASS:
				return &classes;
			case SYMBOL_INTERFACE:
				return &interfaces;
			case SYMBOL_ENUM:
				return &enums;
			case SYMBOL_ENUM_CONSTANT:
				return &enumConstants;
			case SYMBOL_METHOD:
				return &methods;
			case SYMBOL_FIELD:
				return &fields;
			case SYMBOL_TYPE_PARAMETER:
				return &typeParameters;
			default:
				break;
			}
			return nullptr;
		}

		virtual Id recordSymbol(
			const NameHierarchy& symbolName, SymbolKind symbolKind,
			AccessKind access = ACCESS_NONE, bool isImplicit = false)
		{
			std::vector<std::string>* bin = getBinForSymbolKind(symbolKind);
			if (bin != nullptr)
			{
				bin->push_back(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access));
			}
			return 0;
		}

		virtual Id recordSymbol(
			const NameHierarchy& symbolName, SymbolKind symbolKind,
			const ParseLocation& location,
			AccessKind access = ACCESS_NONE, bool isImplicit = false)
		{
			std::vector<std::string>* bin = getBinForSymbolKind(symbolKind);
			if (bin != nullptr)
			{
				bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location));
			}
			return 0;
		}

		virtual Id recordSymbol(
			const NameHierarchy& symbolName, SymbolKind symbolKind,
			const ParseLocation& location, const ParseLocation& scopeLocation,
			AccessKind access = ACCESS_NONE, bool isImplicit = false)
		{
			std::vector<std::string>* bin = getBinForSymbolKind(symbolKind);
			if (bin != nullptr)
			{
				bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location, scopeLocation));
			}
			return 0;
		}

		void recordReference(
			ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
			const ParseLocation& location)
		{
			std::vector<std::string>* referenceContainer = nullptr;
			switch (referenceKind)
			{
			case REFERENCE_TYPE_USAGE:
				referenceContainer = &typeUses;
				break;
			case REFERENCE_USAGE:
				referenceContainer = &usages;
				break;
			case REFERENCE_CALL:
				referenceContainer = &calls;
				break;
			case REFERENCE_INHERITANCE:
				referenceContainer = &inheritances;
				break;
			case REFERENCE_OVERRIDE:
				referenceContainer = &overrides;
				break;
			case REFERENCE_TEMPLATE_ARGUMENT:
				referenceContainer = &templateArgumentTypes;
				break;
			case REFERENCE_TYPE_ARGUMENT:
				referenceContainer = &typeArguments;
				break;
			case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
				referenceContainer = &templateDefaultArgumentTypes;
				break;
			case REFERENCE_TEMPLATE_SPECIALIZATION_OF:
				referenceContainer = &templateSpecializations;
				break;
			case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION_OF:
				referenceContainer = &templateMemberSpecializations;
				break;
			case REFERENCE_INCLUDE:
				referenceContainer = &includes;
				break;
			case REFERENCE_IMPORT:
				referenceContainer = &imports;
				break;
			case REFERENCE_MACRO_USAGE:
				referenceContainer = &macroUses;
				break;
			default:
				break;
			}
			if (referenceContainer != nullptr)
			{
				referenceContainer->push_back(addLocationSuffix(
					contextName.getQualifiedNameWithSignature() + " -> " + referencedName.getQualifiedNameWithSignature(), location)
				);
			}
		}


















		virtual void onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed)
		{
			errors.push_back(addLocationSuffix(message, location));
		}

		virtual void onTypedefParsed(
			const ParseLocation& location, const NameHierarchy& typedefName, AccessKind access, bool isImplicit
		)
		{
			std::string str = addAccessPrefix(typedefName.getQualifiedName(), access);
			typedefs.push_back(addLocationSuffix(str, location));
		}

		virtual void onClassParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			classes.push_back(addLocationSuffix(addAccessPrefix(nameHierarchy.getQualifiedName(), access), location, scopeLocation));
		}

		virtual void onStructParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			structs.push_back(addLocationSuffix(addAccessPrefix(nameHierarchy.getQualifiedName(), access), location, scopeLocation));
		}

		virtual void onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit)
		{
			globalVariables.push_back(addLocationSuffix(variable.getQualifiedName(), location));
		}

		virtual void onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessKind access, bool isImplicit)
		{
			fields.push_back(addLocationSuffix(addAccessPrefix(field.getQualifiedName(), access), location));
		}

		virtual void onFunctionParsed(
			const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit)
		{
			functions.push_back(addLocationSuffix(function.getQualifiedNameWithSignature(), location, scopeLocation));
		}

		virtual void onMethodParsed(
			const ParseLocation& location, const NameHierarchy& method, AccessKind access, AbstractionType abstraction,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			std::string str = method.getQualifiedNameWithSignature();
			str = addAbstractionPrefix(str, abstraction);
			str = addAccessPrefix(str, access);
			str = addLocationSuffix(str, location, scopeLocation);
			methods.push_back(str);
		}

		virtual void onNamespaceParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation, bool isImplicit)
		{
			namespaces.push_back(addLocationSuffix(nameHierarchy.getQualifiedName(), location, scopeLocation));
		}

		virtual void onEnumParsed(
			const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
			const ParseLocation& scopeLocation, bool isImplicit)
		{
			enums.push_back(addLocationSuffix(addAccessPrefix(nameHierarchy.getQualifiedName(), access), location, scopeLocation));
		}

		virtual void onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit)
		{
			enumConstants.push_back(addLocationSuffix(nameHierarchy.getQualifiedName(), location));
		}

		virtual void onTemplateParameterTypeParsed(
			const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit)
		{
			templateParameterTypes.push_back(
				addLocationSuffix(templateParameterTypeNameHierarchy.getQualifiedName(), location)
			);
		}

		virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
		{
			localSymbols.push_back(addLocationSuffix(name, location));
		}

		virtual void onFileParsed(const FileInfo& fileInfo)
		{
			files.push_back(fileInfo.path.str());
		}

		virtual void onMacroExpandParsed(const ParseLocation& location, const NameHierarchy& macroNameHierarchy)
		{
			macroUses.push_back(addLocationSuffix(macroNameHierarchy.getQualifiedName() ,location));
		}

		virtual void onCommentParsed(const ParseLocation& location)
		{
			comments.push_back(addLocationSuffix("comment", location));

		}

		virtual void onInheritanceParsed(
			const ParseLocation& location, const NameHierarchy& childNameHierarchy,
			const NameHierarchy& parentNameHierarchy)
		{
			std::string str = childNameHierarchy.getQualifiedName() + " : " + parentNameHierarchy.getQualifiedName();
			inheritances.push_back(addLocationSuffix(str, location));
		}

		virtual void onMethodOverrideParsed(
			const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider)
		{
			overrides.push_back(addLocationSuffix(overridden.getQualifiedNameWithSignature() + " -> " + overrider.getQualifiedNameWithSignature(), location));
		}

		virtual void onCallParsed(
			const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee)
		{
			calls.push_back(addLocationSuffix(caller.getQualifiedNameWithSignature() + " -> " + callee.getQualifiedNameWithSignature(), location));
		}

		virtual void onUsageParsed(
			const ParseLocation& location, const NameHierarchy& userName, SymbolKind usedType, const NameHierarchy& usedName)
		{
			usages.push_back(addLocationSuffix(userName.getQualifiedNameWithSignature() + " -> " + usedName.getQualifiedName(), location));
		}

		virtual void onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used)
		{
			typeUses.push_back(addLocationSuffix(user.getQualifiedNameWithSignature() + " -> " + used.getQualifiedNameWithSignature(), location));
		}

		virtual void onTemplateArgumentTypeParsed(
			const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
			const NameHierarchy& templateNameHierarchy)
		{
			templateArgumentTypes.push_back(
				addLocationSuffix(templateNameHierarchy.getQualifiedNameWithSignature() + "->" + argumentTypeNameHierarchy.getQualifiedName(), location)
			);
		}

		virtual void onTemplateDefaultArgumentTypeParsed(
			const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
			const NameHierarchy& templateParameterNameHierarchy)
		{
			templateDefaultArgumentTypes.push_back(
				addLocationSuffix(defaultArgumentTypeNameHierarchy.getQualifiedNameWithSignature() + " -> " + templateParameterNameHierarchy.getQualifiedName(), location)
			);
		}

		virtual void onTemplateSpecializationParsed(
			const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
			const NameHierarchy& specializedFromNameHierarchy)
		{
			templateSpecializations.push_back(addLocationSuffix(
				specializedNameHierarchy.getQualifiedNameWithSignature() + " -> " + specializedFromNameHierarchy.getQualifiedNameWithSignature(), location
			));
		}

		virtual void onTemplateMemberFunctionSpecializationParsed(
			const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction)
		{
			templateMemberSpecializations.push_back(addLocationSuffix(
				instantiatedFunction.getQualifiedNameWithSignature() + " -> " + specializedFunction.getQualifiedNameWithSignature(), location
			));
		}

		virtual void onFileIncludeParsed(
			const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo)
		{
			includes.push_back(fileInfo.path.str() + " -> " + includedFileInfo.path.str());
		}

		virtual void onMacroDefineParsed(
			const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation)
		{
			macros.push_back(addLocationSuffix(macroNameHierarchy.getQualifiedName(), location, scopeLocation));
		}

		std::vector<std::string> errors;

		std::vector<std::string> packages;
		std::vector<std::string> typedefs;
		std::vector<std::string> classes;
		std::vector<std::string> interfaces;
		std::vector<std::string> enums;
		std::vector<std::string> enumConstants;
		std::vector<std::string> functions;
		std::vector<std::string> fields;
		std::vector<std::string> globalVariables;
		std::vector<std::string> methods;
		std::vector<std::string> namespaces;
		std::vector<std::string> structs;
		std::vector<std::string> macros;
		std::vector<std::string> templateParameterTypes;
		std::vector<std::string> typeParameters;
		std::vector<std::string> localSymbols;
		std::vector<std::string> files;
		std::vector<std::string> comments;

		std::vector<std::string> inheritances;
		std::vector<std::string> overrides;
		std::vector<std::string> calls;
		std::vector<std::string> usages;	// for variables
		std::vector<std::string> typeUses;	// for types
		std::vector<std::string> macroUses;
		std::vector<std::string> templateArgumentTypes;
		std::vector<std::string> typeArguments;
		std::vector<std::string> templateDefaultArgumentTypes;
		std::vector<std::string> templateSpecializations;
		std::vector<std::string> templateMemberSpecializations;
		std::vector<std::string> includes;
		std::vector<std::string> imports;

	};

	void setupJavaEnvironmentFactory()
	{
		if (!JavaEnvironmentFactory::getInstance())
		{
			std::string errorString;
#ifdef _WIN32
			const std::string separator = ";";
#else
			const std::string separator = ":";
#endif
			JavaEnvironmentFactory::createInstance(
				"../app/data/java/guava-18.0.jar" + separator +
				"../app/data/java/java-indexer.jar" + separator +
				"../app/data/java/javaparser-core.jar" + separator +
				"../app/data/java/javaslang-2.0.3.jar" + separator +
				"../app/data/java/javassist-3.19.0-GA.jar" + separator +
				"../app/data/java/java-symbol-solver-core.jar" + separator +
				"../app/data/java/java-symbol-solver-logic.jar" + separator +
				"../app/data/java/java-symbol-solver-model.jar" + separator,
				errorString
			);
		}
	}

	std::shared_ptr<TestParserClient> parseCode(std::string code, bool logErrors = true)
	{
		NameHierarchy::setDelimiter(".");

		m_args.logErrors = logErrors;
		m_args.language = "Java";
		m_args.languageStandard = "1.8";

		TestFileManager fm;
		std::shared_ptr<FileRegister> fr = std::make_shared<FileRegister>(&fm, false);
		std::shared_ptr<TestParserClient> client = std::make_shared<TestParserClient>();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(code);

		setupJavaEnvironmentFactory();

		JavaParser parser(client.get());
		parser.parseFile("input.cc", textAccess, m_args);

		return client;
	}

	Parser::Arguments m_args;
};
