#include <cxxtest/TestSuite.h>

#include "JavaEnvironmentFactory.h"
#include "JavaParser.h"

#include "TestParserClient.h"

#include "ApplicationSettings.h"
#include "utilityPathDetection.h"
#include "TextAccess.h"
#include "utility.h"
#include "utilityJava.h"

class JavaParserTestSuite: public CxxTest::TestSuite
{
public:
///////////////////////////////////////////////////////////////////////////////
// test finding symbol definitions and declarations

	void test_java_parser_finds_all_jar_dependencies()
	{
		for (const std::wstring& jarName : utility::getRequiredJarNames())
		{
			FilePath jarPath = FilePath(L"../app/data/java/lib/").concatenate(jarName);
			TSM_ASSERT(L"Jar dependency path does not exist: " + jarPath.wstr(), jarPath.exists());
		}
	}

	void test_java_parser_can_setup_environment_factory()
	{
		std::vector<FilePath> javaPaths = utility::getJavaRuntimePathDetector()->getPaths();
		if (!javaPaths.empty())
		{
			ApplicationSettings::getInstance()->setJavaPath(javaPaths[0]);
		}

		const std::string errorString = setupJavaEnvironmentFactory();

		TS_ASSERT_EQUALS("", errorString);

		// if this one fails, maybe your java_path in the test settings is wrong.
		TS_ASSERT_LESS_THAN_EQUALS(1, JavaEnvironmentFactory::getInstance().use_count());
	}

	void test_java_parser_finds_package_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->packages, L"foo <1:9 1:11>"
		));
	}

	void test_java_parser_finds_anotation_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public @interface SampleAnnotation\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->annotations, L"public SampleAnnotation <1:1 <1:19 1:34> 3:1>"
		));
	}

	void test_java_parser_finds_anotation_member_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public @interface SampleAnnotation\n"
			"{\n"
			"	public int value() default 0;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"public int SampleAnnotation.value <3:13 3:17>"
		));
	}

	void test_java_parser_finds_class_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"public A <1:1 <1:14 1:14> 3:1>"
		));
	}

	void test_java_parser_finds_interface_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public interface A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->interfaces, L"public A <1:1 <1:18 1:18> 3:1>"
		));
	}

	void test_java_parser_finds_class_declaration_in_named_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"public foo.A <2:1 <2:14 2:14> 4:1>"
		));
	}

	void test_java_parser_finds_class_declaration_in_nested_named_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo.bar;\n"
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"public foo.bar.A <2:1 <2:14 2:14> 4:1>"
		));
	}

	void test_java_parser_finds_enum_declaration_in_named_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public enum A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enums, L"public foo.A <2:1 <2:13 2:13> 4:1>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->enumConstants, L"foo.A.A_TEST <4:2 4:7>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public foo.A.A() <4:2 <4:2  <4:9 4:9> 4:11> 6:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public void foo.A.bar(foo.A) <4:2 <4:2  <4:14 4:16> 4:21> 6:2>"
		));
	}

	void test_java_parser_finds_anonymous_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	interface I { }\n"
			"	public void bar()\n"
			"	{\n"
			"		I i = new I() { };\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"foo.A.bar.anonymous class (input.cc<7:17>) <7:17 <7:17 7:17> 7:19>"
		));
	}

	void test_java_parser_finds_method_declaration_in_anonymous_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	interface I {\n"
			"		public void foo();\n"
			"	}\n"
			"	public void bar()\n"
			"	{\n"
			"		I i = new I()\n"
			"		{\n"
			"			public void foo() {}\n"
			"		};\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public void foo.A.bar.anonymous class (input.cc<10:3>).foo() <11:4 <11:4  <11:16 11:18> 11:20> 11:23>"
		));
	}

	void test_java_parser_finds_method_declaration_with_static_keyword_in_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	static public void bar()\n"
			"	{\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->methods, L"public static void foo.A.bar() <4:2 <4:2  <4:21 4:23> 4:25> 6:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"default int foo.A.bar <4:6 4:8>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"public int foo.A.bar <4:13 4:15>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"protected int foo.A.bar <4:16 4:18>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"private int foo.A.bar <4:14 4:16>"
		));
	}

	void test_java_parser_finds_static_keyword_in_field_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class A\n"
			"{\n"
			"	static int bar;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"default static int foo.A.bar <4:13 4:15>"
		));
	}

	void test_java_parser_finds_declaration_of_method_parameter()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"foo.A.bar<0> <4:15 4:15>"
		));
	}

	void test_java_parser_finds_declaration_of_local_variable()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"foo.A.bar<0> <6:7 6:7>"
		));
	}

	void test_java_parser_finds_declaration_of_type_parameter_of_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeParameters, L"A<T>.T <1:17 1:17>"
		));
	}

	void test_java_parser_finds_declaration_of_type_parameter_of_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public <T> void foo()\n"
			"	{\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeParameters, L"A.foo<T>.T <3:10 3:10>"
		));
	}

	void test_java_parser_finds_field_of_interface_to_be_implicitly_static()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public interface A\n"
			"{\n"
			"	int b = 5;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->fields, L"default static int A.b <3:6 3:6>"
		));
	}

	void test_java_parser_finds_line_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"// this is a line comment\n"
			"package foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->comments, L"comment <1:1 1:25>"
		));
	}

	void test_java_parser_finds_block_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"/* this is a line comment*/\n"
			"package foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->comments, L"comment <1:1 1:27>"
		));
	}

	void test_java_parser_finds_missing_semicolon_as_parse_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->errors, L"Syntax error on token \"foo\", ; expected after this token <1:9 1:11>"
		));
	}

	void test_java_parser_finds_missing_import_as_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"import foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->errors, L"The import foo cannot be resolved <1:8 1:10>"
		));
	}




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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"public foo.bar.A.B <4:2 <4:15 4:15> 6:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->classes, L"default foo.bar.A.bar.B <6:3 <6:9 6:9> 8:3>"
		));
	}


///////////////////////////////////////////////////////////////////////////////
// test finding qualifier locations

	void test_java_parser_finds_no_qualifier_location_of_standalone_this_expression()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	public void bar()\n"
			"	{\n"
			"		X x = this;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->qualifiers.size(), 0);
	}

	void test_java_parser_finds_qualifier_location_of_import_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"import foo.bar;\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo <1:8 1:10>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_simple_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo.bar;\n"
			"public class A\n"
			"{\n"
			"	public void bar(int i)\n"
			"	{\n"
			"		foo.bar.A a;\n"
			"	};\n"
			"};\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo <6:3 6:5>"
			));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.bar <6:7 6:9>"
			));
	}

	void test_java_parser_finds_qualifier_location_of_field_access()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	public int i;\n"
			"	\n"
			"	public void bar()\n"
			"	{\n"
			"		this.i = 9;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.X <8:3 8:6>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_super_field_access()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"class A\n"
			"{\n"
			"	int a;\n"
			"}\n"
			"\n"
			"class B extends A\n"
			"{\n"
			"	void foo()\n"
			"	{\n"
			"		B.super.a = 0;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.B <11:3 11:3>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.A <11:5 11:9>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_this_expression()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"class A\n"
			"{\n"
			"	int a;\n"
			"	\n"
			"	void foo()\n"
			"	{\n"
			"		A a = A.this;"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.A <8:9 8:9>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_method_invocation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	public static void bar()\n"
			"	{\n"
			"		foo.X.bar();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo <6:3 6:5>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.X <6:7 6:7>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_method_invocation_on_this()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	public void bar()\n"
			"	{\n"
			"		this.bar();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.X <6:3 6:6>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_super_method_invocation()
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
			"			foo.X.B.super.bar();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo <15:4 15:6>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.X <15:8 15:8>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.X.B <15:10 15:10>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"foo.X.A <15:12 15:16>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_creation_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Functor\n"
			"	{\n"
			"		public void doSomething();\n"
			"	}\n"
			"	\n"
			"	public class Bar\n"
			"	{\n"
			"	}\n"
			"	\n"
			"	void foo()\n"
			"	{\n"
			"		Functor method = A.Bar::new;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"A <14:20 14:20>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"A.Bar <14:22 14:24>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_expression_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Functor\n"
			"	{\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B\n"
			"	{\n"
			"		void bar()\n"
			"		{\n"
			"		}\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Functor method = B::bar;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"A.B <17:20 17:20>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_super_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	public interface Functor {\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B {\n"
			"		void bar() {\n"
			"		}\n"
			"	}\n"
			"\n"
			"	public class C extends B {\n"
			"		void foo() {\n"
			"			Functor method = super::bar;\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"A.B <13:21 13:25>"
		));
	}

	void test_java_parser_finds_qualifier_location_of_class_instance_creation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	public interface Functor {\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B {\n"
			"		void bar() {\n"
			"			B b = new A.B();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->qualifiers, L"A <8:14 8:14>"
		));
	}


///////////////////////////////////////////////////////////////////////////////
// test finding usages of symbols

	void test_java_parser_finds_usage_of_marker_annotation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public @interface SampleAnnotation\n"
			"{\n"
			"}\n"
			"\n"
			"@SampleAnnotation\n"
			"class Foo\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->annotationUses, L"Foo -> SampleAnnotation <5:2 5:17>"
		));
	}

	void test_java_parser_finds_usage_of_single_member_annotation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public @interface SampleAnnotation\n"
			"{\n"
			"	public int value() default 0;\n"
			"}\n"
			"\n"
			"@SampleAnnotation(33)\n"
			"class Foo\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->annotationUses, L"Foo -> SampleAnnotation <6:2 6:17>"
		));
	}

	void test_java_parser_finds_usage_of_normal_annotation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public @interface SampleAnnotation\n"
			"{\n"
			"	public int a() default 0;\n"
			"	public int b() default 0;\n"
			"}\n"
			"\n"
			"@SampleAnnotation()\n"
			"class Foo\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->annotationUses, L"Foo -> SampleAnnotation <7:2 7:17>"
		));
	}

	void test_java_parser_finds_usage_of_normal_annotation_member_in_initialization()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public @interface SampleAnnotation\n"
			"{\n"
			"	public int a() default 0;\n"
			"	public int b() default 0;\n"
			"}\n"
			"\n"
			"@SampleAnnotation(a = 9)\n"
			"class Foo\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"Foo -> int SampleAnnotation.a <7:19 7:19>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"foo.B -> foo.A <6:24 6:24>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"foo.B -> foo.A <6:27 6:27>"
		));
	}

	void test_java_parser_finds_inheritance_of_anonymous_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Base\n"
			"	{\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Base b = new Base()\n"
			"		{\n"
			"		}\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->inheritances, L"A.foo.anonymous class (input.cc<10:3>) -> A.Base <9:16 9:19>"
		));
	}

	void test_java_parser_finds_usage_of_string_for_var_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public void foo(){\n"
			"		var a = \"test\";\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.foo() -> java.lang.String <4:3 4:5>"
		));
	}

	void test_java_parser_finds_type_parameter_in_signature_of_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"	public A<Void> foo(A<Void> a){\n"
			"		return a;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<java.lang.Void> A<T>.foo(A<java.lang.Void>) -> A<T> <3:9 3:9>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<java.lang.Void> A<T>.foo(A<java.lang.Void>) -> A<T> <3:21 3:21>"
		));
	}

	void test_parser_finds_usage_of_type_defined_in_base_class()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"Foo.Base.X Foo.Derived.x -> Foo.Base.X <7:10 7:10>"
		));
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
			"		A.B b = new A.B();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.bar() -> A.B <8:5 8:5>"
		));
	}

	void test_java_parser_finds_type_argument_of_parameterized_type()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T> {\n"
			"	T t;\n"
			"}\n"
			"public class B {\n"
			"	void foo() {\n"
			"		A<Void> a = null;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"A<T> -> java.lang.Void <6:5 6:8>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void B.foo() -> java.lang.Void <6:5 6:8>"
		));
	}

	void test_java_parser_finds_type_argument_of_method_invocation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	public static void foo() {\n"
			"	}\n"
			"\n"
			"	public static void bar() {\n"
			"		foo.X.<Void>foo();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"static void foo.X.foo() -> java.lang.Void <8:10 8:13>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"static void foo.X.bar() -> java.lang.Void <8:10 8:13>"
		));
	}

	void test_java_parser_finds_type_argument_of_super_method_invocation()
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
			"			super.<Void>bar();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"void foo.X.A.bar() -> java.lang.Void <15:11 15:14>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void foo.X.B.bar() -> java.lang.Void <15:11 15:14>"
		));
	}

	void test_java_parser_finds_type_argument_of_constructor_invocation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class Bar\n"
			"{\n"
			"	public Bar()\n"
			"	{\n"
			"	}\n"
			"	\n"
			"	public Bar(int i)\n"
			"	{\n"
			"		<Void>this();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"foo.Bar.Bar() -> java.lang.Void <10:4 10:7>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"foo.Bar.Bar(int) -> java.lang.Void <10:4 10:7>"
		));
	}

	void test_java_parser_finds_type_argument_of_super_constructor_invocation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public class Base\n"
			"	{\n"
			"	}\n"
			"\n"
			"	public class Derived extends Base\n"
			"	{\n"
			"		public Derived()\n"
			"		{\n"
			"			<Void>super();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"A.Base.Base() -> java.lang.Void <11:5 11:8>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A.Derived.Derived() -> java.lang.Void <11:5 11:8>"
		));
	}

	void test_java_parser_finds_type_argument_of_creation_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Functor\n"
			"	{\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B\n"
			"	{\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Functor method = B::<Void>new;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"A.B.B() -> java.lang.Void <14:24 14:27>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.foo() -> java.lang.Void <14:24 14:27>"
		));
	}

	void test_java_parser_finds_type_argument_of_expression_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Functor\n"
			"	{\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	static void bar()\n"
			"	{\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Functor method = A::<Void>bar;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"static void A.bar() -> java.lang.Void <14:24 14:27>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.foo() -> java.lang.Void <14:24 14:27>"
		));
	}

	void test_java_parser_finds_type_argument_of_super_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	public interface Functor {\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B {\n"
			"		<T> void bar() {\n"
			"		}\n"
			"	}\n"
			"\n"
			"	public class C extends B {\n"
			"		void foo() {\n"
			"			Functor method = super::<Void>bar;\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"void A.B.bar<T>() -> java.lang.Void <13:29 13:32>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.C.foo() -> java.lang.Void <13:29 13:32>"
		));
	}

	void test_java_parser_finds_type_argument_of_type_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	\n"
			"	void foo() {\n"
			"		Functor method = int []::<Void>clone;\n"
			"	}\n"
			"}\n"
		);

		// currently we cannot record the typeArguments of array type methods

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.foo() -> java.lang.Void <4:29 4:32>"
		));
	}

	void test_java_parser_finds_type_argument_of_class_instance_creation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	public interface Functor {\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B {\n"
			"		<T> B() {\n"
			"		}\n"
			"		void bar() {\n"
			"			B b = new <Void>A.B();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeArguments, L"A.B.B<T>() -> java.lang.Void <10:15 10:18>"
		));

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.B.bar() -> java.lang.Void <10:15 10:18>"
		));
	}

	void test_java_parser_finds_super_method_invocation()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void foo.X.B.bar() -> void foo.X.A.bar() <15:10 15:12>"
		));
	}

	void test_java_parser_finds_constructor_invocation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class Bar\n"
			"{\n"
			"	public Bar()\n"
			"	{\n"
			"	}\n"
			"	\n"
			"	public Bar(int i)\n"
			"	{\n"
			"		this();\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"foo.Bar.Bar(int) -> foo.Bar.Bar() <10:3 10:6>"
		));
	}

	void test_java_parser_finds_super_constructor_invocation()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public class Base\n"
			"	{\n"
			"	}\n"
			"\n"
			"	public class Derived extends Base\n"
			"	{\n"
			"		public Derived()\n"
			"		{\n"
			"			super();\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"A.Derived.Derived() -> A.Base.Base() <11:4 11:8>"
		));
	}

	void test_java_parser_finds_invocation_of_method_of_anonymous_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Main {\n"
			"	public interface Interfaze {\n"
			"		public void foo();\n"
			"	}\n"
			"\n"
			"	private Interfaze i = new Interfaze() {\n"
			"		public void foo() {\n"
			"			bar();\n"
			"		}\n"
			"\n"
			"		private void bar() {\n"
			"		}\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->calls, L"void Main.anonymous class (input.cc<6:40>).foo() -> void Main.anonymous class (input.cc<6:40>).bar() <8:4 8:6>"
		));
	}

	void test_java_parser_finds_overridden_method_with_same_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Main {\n"
			"	public interface Interfaze {\n"
			"		public void foo(int t);\n"
			"	}\n"
			"\n"
			"	public class C implements Interfaze {\n"
			"		public void foo(int t) {\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->overrides, L"void Main.C.foo(int) -> void Main.Interfaze.foo(int) <7:15 7:17>"
		));
	}

	void test_java_parser_finds_overridden_method_with_generic_signature()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"class Main <X> {\n"
			"	public interface Interfaze <T> {\n"
			"		public void foo(T t);\n"
			"	}\n"
			"\n"
			"	public class C implements Interfaze <X> {\n"
			"		public void foo(X t) {\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->overrides, L"void Main<X>.C.foo(Main<X>.X) -> void Main<X>.Interfaze<T>.foo(Main<X>.Interfaze<T>.T) <7:15 7:17>"
		));
	}

	void test_java_parser_finds_method_usage_for_creation_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Functor\n"
			"	{\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B\n"
			"	{\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Functor method = B::new;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A.foo() -> A.B.B() <14:23 14:25>"
		));
	}

	void test_java_parser_finds_method_usage_for_expression_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Functor\n"
			"	{\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B\n"
			"	{\n"
			"		void bar()\n"
			"		{\n"
			"		}\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Functor method = B::bar;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A.foo() -> void A.B.bar() <17:23 17:25>"
		));
	}

	void test_java_parser_finds_method_usage_for_super_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	public interface Functor {\n"
			"		public void doSomething();\n"
			"	}\n"
			"\n"
			"	public class B {\n"
			"		void bar() {\n"
			"		}\n"
			"	}\n"
			"\n"
			"	public class C extends B {\n"
			"		void foo() {\n"
			"			Functor method = super::bar;\n"
			"		}\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void A.C.foo() -> void A.B.bar() <13:28 13:30>"
		));
	}

	void test_java_parser_finds_no_method_usage_for_type_method_reference()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A {\n"
			"	void foo() {\n"
			"		Functor method = int []::clone;\n"
			"	}\n"
			"}\n"
		);

		// finding method usage here may be implemented in the future.
		TS_ASSERT_EQUALS(client->usages.size(), 0);
	}

	void test_java_parser_finds_no_usage_of_field_within_that_fields_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	private int t;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 0);
	}

	void test_java_parser_finds_no_usage_of_enum_constant_within_that_enum_constants_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public enum X\n"
			"{\n"
			"	OPTION_0;\n"
			"}\n"
		);

		TS_ASSERT_EQUALS(client->usages.size(), 0);
	}

	void test_java_parser_finds_usage_of_field_with_same_name_as_method_parameter()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"foo.X.X(int) -> int foo.X.t <7:8 7:8>"
		));
	}

	void test_java_parser_does_not_confuse_method_name_with_field_name()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
			"public class X\n"
			"{\n"
			"	private int foo;\n"
			"	public void foo()\n"
			"	{\n"
			"		this.foo = 5;\n"
			"	}\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->usages, L"void foo.X.foo() -> int foo.X.foo <7:8 7:10>"
		));
	}

	void test_java_parser_finds_assignment_of_method_parameter()
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"foo.A.bar<0> <6:3 6:3>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"foo.A.bar<0> <7:3 7:3>"
		));
	}

	void test_java_parser_finds_scope_of_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
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

	void test_java_parser_finds_scope_of_enum_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public enum A\n"
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:2> <4:2 4:2>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:2> <5:2 5:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:2> <4:2 4:2>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<4:2> <5:2 5:2>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<6:3> <6:3 6:3>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<6:3> <9:3 9:3>"
		));
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

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<5:3> <5:3 5:3>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<5:3> <6:3 6:3>"
		));
	}

	void test_java_parser_finds_scope_of_array_initialization_list()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	private int[] array = {1, 2};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:24> <3:24 3:24>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<3:24> <3:29 3:29>"
		));
	}

	void test_java_parser_finds_scope_of_anonymous_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public interface Base\n"
			"	{\n"
			"	}\n"
			"\n"
			"	void foo()\n"
			"	{\n"
			"		Base b = new Base()\n"
			"		{\n"
			"		}\n"
			"	};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<10:3> <10:3 10:3>"
		));
		TS_ASSERT(utility::containsElement<std::wstring>(
			client->localSymbols, L"input.cc<10:3> <11:3 11:3>"
		));
	}

	void test_java_parser_finds_usage_of_type_parameter_of_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"	T t;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<T>.T A<T>.t -> A<T>.T <3:2 3:2>"
		));
	}

	void test_java_parser_finds_usage_of_type_parameter_of_method()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"	public <T> void foo(T t){};\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"void A.foo<T>(T) -> A.foo<T>.T <3:22 3:22>"
		));
	}

	void test_java_parser_finds_correct_location_of_generic_type_usage()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"	A<Void> t;\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<java.lang.Void> A<T>.t -> A<T> <3:2 3:2>"
		));
	}

	void test_java_parser_finds_bound_type_of_type_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T extends Void>\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::wstring>(
			client->typeUses, L"A<T>.T -> java.lang.Void <1:27 1:30>"
		));
	}


	//void test_cxx_parser_catches_error()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"int a = b;\n",
	//		false
	//	);

	//	TS_ASSERT_EQUALS(client->errors.size(), 1);
	//	TS_ASSERT_EQUALS(client->errors[0], L"use of undeclared identifier \'b\' <1:9 1:9>");
	//}

	//void test_cxx_parser_finds_location_of_line_comment()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"// this is a line comment\n"
	//	);

	//	TS_ASSERT_EQUALS(client->comments.size(), 1);
	//	TS_ASSERT_EQUALS(client->comments[0], L"comment <1:1 1:26>");
	//}

	//void test_cxx_parser_finds_location_of_block_comment()
	//{
	//	std::shared_ptr<TestParserClient> client = parseCode(
	//		"/* this is a\n"
	//		"block comment */\n"
	//	);

	//	TS_ASSERT_EQUALS(client->comments.size(), 1);
	//	TS_ASSERT_EQUALS(client->comments[0], L"comment <1:1 2:17>");
	//}


private:
	std::string setupJavaEnvironmentFactory()
	{
		if (!JavaEnvironmentFactory::getInstance())
		{
			std::string errorString;
#ifdef _WIN32
			const std::string separator = ";";
#else
			const std::string separator = ":";
#endif
			std::string classPath = "";
			{
				const std::vector<std::wstring> jarNames =  utility::getRequiredJarNames();
				for (size_t i = 0; i < jarNames.size(); i++)
				{
					if (i != 0)
					{
						classPath += separator;
					}
					classPath += FilePath(L"../app/data/java/lib/").concatenate(jarNames[i]).str();
				}
			}

			JavaEnvironmentFactory::createInstance(
				classPath,
				errorString
			);

			return errorString;
		}

		return "";
	}

	std::shared_ptr<TestParserClient> parseCode(std::string code, bool logErrors = true)
	{
		std::shared_ptr<TestParserClient> parserClient = std::make_shared<TestParserClient>();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(code);

		setupJavaEnvironmentFactory();

		JavaParser parser(parserClient, std::make_shared<IndexerStateInfo>());
		parser.buildIndex(FilePath(L"input.cc"), textAccess);

		return parserClient;
	}
};
