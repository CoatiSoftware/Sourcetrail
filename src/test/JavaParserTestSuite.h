#include "cxxtest/TestSuite.h"

#include "settings/ApplicationSettings.h"
#include "utility/utilityPathDetection.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"

#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaParser.h"

#include "helper/TestParserClient.h"

class JavaParserTestSuite: public CxxTest::TestSuite
{
public:
///////////////////////////////////////////////////////////////////////////////
// test finding symbol definitions and declarations

	void test_java_parser_can_setup_environment_factory()
	{
		std::vector<FilePath> javaPaths = utility::getJavaRuntimePathDetector()->getPaths();
		if (!javaPaths.empty())
		{
			ApplicationSettings::getInstance()->setJavaPath(javaPaths[0]);
		}

		setupJavaEnvironmentFactory();

		// if this one fails, maybe your java_path in the test settings is wrong.
		TS_ASSERT_LESS_THAN_EQUALS(1, JavaEnvironmentFactory::getInstance().use_count());
	}

	void test_java_parser_finds_package_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->packages, "foo <1:9 1:11>"
		));
	}

	void test_java_parser_finds_class_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->classes, "public A <1:1 <1:14 1:14> 3:1>"
		));
	}

	void test_java_parser_finds_interface_declaration_in_defaut_package()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public interface A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->interfaces, "public A <1:1 <1:18 1:18> 3:1>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->classes, "public foo.A <2:1 <2:14 2:14> 4:1>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->classes, "public foo.bar.A <2:1 <2:14 2:14> 4:1>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->enums, "public foo.A <2:1 <2:13 2:13> 4:1>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->enumConstants, "foo.A.A_TEST <4:2 4:7>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->methods, "public foo.A.A() <4:2 <4:9 4:9> 6:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->methods, "public void foo.A.bar(foo.A) <4:2 <4:14 4:16> 6:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->classes, "foo.A.bar.anonymous class (input.cc<7:17>) <7:17 <7:17 7:17> 7:19>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->methods, "public void foo.A.bar.anonymous class (input.cc<10:3>).foo() <11:4 <11:16 11:18> 11:23>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->methods, "public static void foo.A.bar() <4:2 <4:21 4:23> 6:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->fields, "default int foo.A.bar <4:6 4:8>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->fields, "public int foo.A.bar <4:13 4:15>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->fields, "protected int foo.A.bar <4:16 4:18>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->fields, "private int foo.A.bar <4:14 4:16>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->fields, "default static int foo.A.bar <4:13 4:15>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "foo.A.bar<0> <4:15 4:15>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "foo.A.bar<0> <6:7 6:7>"
		));
	}

	void test_java_parser_finds_declaration_of_type_parameter_of_class()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T>\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeParameters, "A<T>.T <1:17 1:17>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeParameters, "A.foo<T>.T <3:10 3:10>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->fields, "default static int A.b <3:6 3:6>"
		));
	}

	void test_java_parser_finds_line_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"// this is a line comment\n"
			"package foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->comments, "comment <1:1 1:25>"
		));
	}

	void test_java_parser_finds_block_comment()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"/* this is a line comment*/\n"
			"package foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->comments, "comment <1:1 1:27>"
		));
	}

	void test_java_parser_finds_missing_semicolon_as_parse_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"package foo\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->errors, "Syntax error on token \"foo\", ; expected after this token <1:9 1:11>"
		));
	}

	void test_java_parser_finds_missing_import_as_error()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"import foo;\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->errors, "The import foo cannot be resolved <1:8 1:10>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->classes, "public foo.bar.A.B <4:2 <4:15 4:15> 6:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->classes, "default foo.bar.A.bar.B <6:3 <6:9 6:9> 8:3>"
		));
	}


///////////////////////////////////////////////////////////////////////////////
// test finding usages of symbols

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

		TS_ASSERT(utility::containsElement<std::string>(
			client->inheritances, "foo.B -> foo.A <6:24 6:24>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->inheritances, "foo.B -> foo.A <6:27 6:27>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->inheritances, "A.foo.anonymous class (input.cc<10:3>) -> A.Base <9:16 9:19>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "A<java.lang.Void> A<T>.foo(A<java.lang.Void>) -> A<T> <3:9 3:9>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "A<java.lang.Void> A<T>.foo(A<java.lang.Void>) -> A<T> <3:21 3:21>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "Foo.Base.X Foo.Derived.x -> Foo.Base.X <7:10 7:10>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "void A.bar() -> A.B <8:3 8:5>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->calls, "void foo.X.B.bar() -> void foo.X.A.bar() <15:10 15:12>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->calls, "foo.Bar.Bar(int) -> foo.Bar.Bar() <10:3 10:6>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->calls, "A.Derived.Derived() -> A.Base.Base() <11:4 11:8>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->calls, "void Main.anonymous class (input.cc<6:40>).foo() -> void Main.anonymous class (input.cc<6:40>).bar() <8:4 8:6>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->overrides, "void Main.C.foo(int) -> void Main.Interfaze.foo(int) <7:15 7:17>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->overrides, "void Main<X>.C.foo(Main<X>.X) -> void Main<X>.Interfaze<T>.foo(Main<X>.Interfaze<T>.T) <7:15 7:17>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->usages, "void A.foo() -> A.B.B() <14:23 14:25>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->usages, "void A.foo() -> void A.B.bar() <17:23 17:25>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->usages, "void A.C.foo() -> void A.B.bar() <13:28 13:30>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->usages, "foo.X.X(int) -> int foo.X.t <7:8 7:8>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->usages, "void foo.X.foo() -> int foo.X.foo <7:8 7:10>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "foo.A.bar<0> <6:3 6:3>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "foo.A.bar<0> <7:3 7:3>"
		));
	}

	void test_java_parser_finds_scope_of_class_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<2:1> <2:1 2:1>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<2:1> <3:1 3:1>"
		));
	}

	void test_java_parser_finds_scope_of_enum_declaration()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public enum A\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<2:1> <2:1 2:1>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<2:1> <3:1 3:1>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<4:2> <4:2 4:2>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<4:2> <5:2 5:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<4:2> <4:2 4:2>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<4:2> <5:2 5:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<6:3> <6:3 6:3>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<6:3> <9:3 9:3>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<5:3> <5:3 5:3>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<5:3> <6:3 6:3>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<3:24> <3:24 3:24>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<3:24> <3:29 3:29>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<10:3> <10:3 10:3>"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			client->localSymbols, "input.cc<10:3> <11:3 11:3>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "A<T>.T A<T>.t -> A<T>.T <3:2 3:2>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "void A.foo<T>(T) -> A.foo<T>.T <3:22 3:22>"
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

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "A<java.lang.Void> A<T>.t -> A<T> <3:2 3:2>"
		));
	}

	void test_java_parser_finds_bound_type_of_type_parameter()
	{
		std::shared_ptr<TestParserClient> client = parseCode(
			"public class A <T extends Void>\n"
			"{\n"
			"}\n"
		);

		TS_ASSERT(utility::containsElement<std::string>(
			client->typeUses, "A<T>.T -> java.lang.Void <1:27 1:30>"
		));
		system("PAUSE");
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
			std::string classPath = "";
			{
				const std::vector<std::string> jarNames =  JavaParser::getRequiredJarNames();
				for (size_t i = 0; i < jarNames.size(); i++)
				{
					if (i != 0)
					{
						classPath += separator;
					}
					classPath += "../app/data/java/" + jarNames[i];
				}
			}

			JavaEnvironmentFactory::createInstance(
				classPath,
				errorString
			);
		}
	}

	std::shared_ptr<TestParserClient> parseCode(std::string code, bool logErrors = true)
	{
		std::shared_ptr<TestParserClient> parserClient = std::make_shared<TestParserClient>();

		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromString(code);

		setupJavaEnvironmentFactory();

		JavaParser parser(parserClient, nullptr);
		parser.buildIndex(FilePath("input.cc"), textAccess);

		return parserClient;
	}
};
