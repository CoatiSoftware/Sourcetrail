#include "catch.hpp"

#include "language_packages.h"

#if BUILD_JAVA_LANGUAGE_PACKAGE

#	include "ApplicationSettings.h"
#	include "JavaEnvironmentFactory.h"
#	include "JavaParser.h"
#	include "ParserClientImpl.h"
#	include "TextAccess.h"
#	include "utility.h"
#	include "utilityJava.h"
#	include "utilityPathDetection.h"

#	include "TestIntermediateStorage.h"

#	define REQUIRE_MESSAGE(msg, cond)                                                              \
		do                                                                                         \
		{                                                                                          \
			INFO(msg);                                                                             \
			REQUIRE(cond);                                                                         \
		} while ((void)0, 0)

namespace
{
std::string setupJavaEnvironmentFactory()
{
	if (!JavaEnvironmentFactory::getInstance())
	{
		std::string errorString;
#	ifdef _WIN32
		const std::string separator = ";";
#	else
		const std::string separator = ":";
#	endif
		std::string classPath = "";
		{
			const std::vector<std::wstring> jarNames = utility::getRequiredJarNames();
			for (size_t i = 0; i < jarNames.size(); i++)
			{
				if (i != 0)
				{
					classPath += separator;
				}
				classPath += FilePath(L"../app/data/java/lib/").concatenate(jarNames[i]).str();
			}
		}

		JavaEnvironmentFactory::createInstance(classPath, errorString);

		return errorString;
	}

	return "";
}

std::shared_ptr<TestIntermediateStorage> parseCode(std::string code, bool logErrors = true)
{
	setupJavaEnvironmentFactory();

	std::shared_ptr<TestIntermediateStorage> storage = std::make_shared<TestIntermediateStorage>();
	JavaParser parser(
		std::make_shared<ParserClientImpl>(storage.get()), std::make_shared<IndexerStateInfo>());
	parser.buildIndex(FilePath(L"input.java"), TextAccess::createFromString(code));

	storage->generateStringLists();

	return storage;
}
}	 // namespace

///////////////////////////////////////////////////////////////////////////////
// test finding symbol definitions and declarations

TEST_CASE("java parser finds all jar dependencies")
{
	for (const std::wstring& jarName: utility::getRequiredJarNames())
	{
		FilePath jarPath = FilePath(L"../app/data/java/lib/").concatenate(jarName);
		REQUIRE_MESSAGE("Jar dependency path does not exist: " + jarPath.str(), jarPath.exists());
	}
}

TEST_CASE("java parser can setup environment factory")
{
	std::vector<FilePath> javaPaths = utility::getJavaRuntimePathDetector()->getPaths();
	if (!javaPaths.empty())
	{
		ApplicationSettings::getInstance()->setJavaPath(javaPaths[0]);
	}

	const std::string errorString = setupJavaEnvironmentFactory();

	REQUIRE("" == errorString);

	// if this one fails, maybe your java_path in the test settings is wrong.
	REQUIRE(JavaEnvironmentFactory::getInstance().use_count() >= 1);
}

TEST_CASE("java parser finds package declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode("package foo;\n");

	REQUIRE(utility::containsElement<std::wstring>(client->packages, L"foo <1:9 1:11>"));
}

TEST_CASE("java parser finds anotation declaration in defaut package")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public @interface SampleAnnotation\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->annotations, L"public SampleAnnotation <1:1 <1:19 1:34> 3:1>"));
}

TEST_CASE("java parser finds anotation member declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public @interface SampleAnnotation\n"
		"{\n"
		"	public int value() default 0;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->fields, L"public int SampleAnnotation.value <3:13 3:17>"));
}

TEST_CASE("java parser finds class declaration in defaut package")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->classes, L"public A <1:1 <1:14 1:14> 3:1>"));
}

TEST_CASE("java parser finds interface declaration in defaut package")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public interface A\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->interfaces, L"public A <1:1 <1:18 1:18> 3:1>"));
}

TEST_CASE("java parser finds class declaration in named package")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->classes, L"public foo.A <2:1 <2:14 2:14> 4:1>"));
}

TEST_CASE("java parser finds class declaration in nested named package")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo.bar;\n"
		"public class A\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->classes, L"public foo.bar.A <2:1 <2:14 2:14> 4:1>"));
}

TEST_CASE("java parser finds enum declaration in named package")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public enum A\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->enums, L"public foo.A <2:1 <2:13 2:13> 4:1>"));
}

TEST_CASE("java parser finds enum constant declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public enum A\n"
		"{\n"
		"	A_TEST(0)\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->enumConstants, L"foo.A.A_TEST <4:2 4:7>"));
}

TEST_CASE("java parser finds constructor declaration without parameters")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	public A()\n"
		"	{\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->methods, L"public foo.A.A() <4:2 <4:2 <4:9 4:9> 4:11> 6:2>"));
}

TEST_CASE("java parser finds method declaration with custom type in signature")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	public void bar(A a)\n"
		"	{\n"
		"	};\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->methods, L"public void foo.A.bar(foo.A) <4:2 <4:2 <4:14 4:16> 4:21> 6:2>"));
}

TEST_CASE("java parser finds anonymous class declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	interface I { }\n"
		"	public void bar()\n"
		"	{\n"
		"		I i = new I() { };\n"
		"	};\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->classes, L"foo.A.bar.anonymous class (input.java<7:17>) <7:17 <7:17 7:17> 7:19>"));
}

TEST_CASE("java parser finds method declaration in anonymous class")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->methods,
		L"public void foo.A.bar.anonymous class (input.java<10:3>).foo() <11:4 <11:4 <11:16 11:18> "
		L"11:20> 11:23>"));
}

TEST_CASE("java parser finds method declaration with static keyword in signature")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	static public void bar()\n"
		"	{\n"
		"	};\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->methods, L"public static void foo.A.bar() <4:2 <4:2 <4:21 4:23> 4:25> 6:2>"));
}

TEST_CASE("java parser finds field declaration with initial assignment")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	int bar = 0;\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->fields, L"default int foo.A.bar <4:6 4:8>"));
}

TEST_CASE("java parser finds public access specifier in field declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	public int bar;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->fields, L"public int foo.A.bar <4:13 4:15>"));
}

TEST_CASE("java parser finds protected access specifier in field declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	protected int bar;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->fields, L"protected int foo.A.bar <4:16 4:18>"));
}

TEST_CASE("java parser finds private access specifier in field declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	private int bar;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->fields, L"private int foo.A.bar <4:14 4:16>"));
}

TEST_CASE("java parser finds static keyword in field declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	static int bar;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->fields, L"default static int foo.A.bar <4:13 4:15>"));
}

TEST_CASE("java parser finds declaration of method parameter")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	void bar(int i)\n"
		"	{\n"
		"	}\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"foo.A.bar<0> <4:15 4:15>"));
}

TEST_CASE("java parser finds declaration of local variable")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	void bar()\n"
		"	{\n"
		"		int i;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->localSymbols, L"foo.A.bar<0> <6:7 6:7>"));
}

TEST_CASE("java parser finds declaration of type parameter of class")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A <T>\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->typeParameters, L"A<T>.T <1:17 1:17>"));
}

TEST_CASE("java parser finds declaration of type parameter of method")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public <T> void foo()\n"
		"	{\n"
		"	}\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->typeParameters, L"A.foo<T>.T <3:10 3:10>"));
}

TEST_CASE("java parser finds field of interface to be implicitly static")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public interface A\n"
		"{\n"
		"	int b = 5;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->fields, L"default static int A.b <3:6 3:6>"));
}

TEST_CASE("java parser finds line comment")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"// this is a line comment\n"
		"package foo;\n");

	REQUIRE(utility::containsElement<std::wstring>(client->comments, L"comment <1:1 1:25>"));
}

TEST_CASE("java parser finds block comment")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"/* this is a line comment*/\n"
		"package foo;\n");

	REQUIRE(utility::containsElement<std::wstring>(client->comments, L"comment <1:1 1:27>"));
}

TEST_CASE("java parser finds missing semicolon as parse error")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode("package foo\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->errors, L"Syntax error on token \"foo\", ; expected after this token <1:9 1:9>"));
}

TEST_CASE("java parser finds missing import as error")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode("import foo;\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->errors, L"The import foo cannot be resolved <1:8 1:8>"));
}


///////////////////////////////////////////////////////////////////////////////
// test finding nested symbol definitions and declarations

TEST_CASE("java parser finds class declaration nested in class")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo.bar;\n"
		"public class A\n"
		"{\n"
		"	public class B\n"
		"	{\n"
		"	}\n"
		"};\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->classes, L"public foo.bar.A.B <4:2 <4:15 4:15> 6:2>"));
}

TEST_CASE("java parser finds class declaration nested in method")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo.bar;\n"
		"public class A\n"
		"{\n"
		"	public void bar(int i)\n"
		"	{\n"
		"		class B\n"
		"		{\n"
		"		}\n"
		"	};\n"
		"};\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->classes, L"default foo.bar.A.bar.B <6:3 <6:9 6:9> 8:3>"));
}


///////////////////////////////////////////////////////////////////////////////
// test finding qualifier locations

TEST_CASE("java parser finds no qualifier location of standalone this expression")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	public void bar()\n"
		"	{\n"
		"		X x = this;\n"
		"	}\n"
		"}\n");

	REQUIRE(client->qualifiers.size() == 0);
}

TEST_CASE("java parser finds qualifier location of import declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode("import foo.bar;\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo <1:8 1:10>"));
}

TEST_CASE("java parser finds qualifier location of simple type")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo.bar;\n"
		"public class A\n"
		"{\n"
		"	public void bar(int i)\n"
		"	{\n"
		"		foo.bar.A a;\n"
		"	};\n"
		"};\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo <6:3 6:5>"));
	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.bar <6:7 6:9>"));
}

TEST_CASE("java parser finds qualifier location of field access")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	public int i;\n"
		"	\n"
		"	public void bar()\n"
		"	{\n"
		"		this.i = 9;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.X <8:3 8:6>"));
}

TEST_CASE("java parser finds qualifier location of super field access")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.B <11:3 11:3>"));

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.A <11:5 11:9>"));
}

TEST_CASE("java parser finds qualifier location of this expression")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"class A\n"
		"{\n"
		"	int a;\n"
		"	\n"
		"	void foo()\n"
		"	{\n"
		"		A a = A.this;"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.A <8:9 8:9>"));
}

TEST_CASE("java parser finds qualifier location of method invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	public static void bar()\n"
		"	{\n"
		"		foo.X.bar();\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo <6:3 6:5>"));

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.X <6:7 6:7>"));
}

TEST_CASE("java parser finds qualifier location of method invocation on this")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	public void bar()\n"
		"	{\n"
		"		this.bar();\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.X <6:3 6:6>"));
}

TEST_CASE("java parser finds qualifier location of super method invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo <15:4 15:6>"));

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.X <15:8 15:8>"));

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.X.B <15:10 15:10>"));

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"foo.X.A <15:12 15:16>"));
}

TEST_CASE("java parser finds qualifier location of creation reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"A <14:20 14:20>"));

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"A.Bar <14:22 14:24>"));
}

TEST_CASE("java parser finds qualifier location of expression method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"A.B <17:20 17:20>"));
}

TEST_CASE("java parser finds qualifier location of super method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"A.B <13:21 13:25>"));
}

TEST_CASE("java parser finds qualifier location of class instance creation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->qualifiers, L"A <8:14 8:14>"));
}


///////////////////////////////////////////////////////////////////////////////
// test finding usages of symbols

TEST_CASE("java parser finds usage of marker annotation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public @interface SampleAnnotation\n"
		"{\n"
		"}\n"
		"\n"
		"@SampleAnnotation\n"
		"class Foo\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->annotationUses, L"Foo -> SampleAnnotation <5:2 5:17>"));
}

TEST_CASE("java parser finds usage of single member annotation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public @interface SampleAnnotation\n"
		"{\n"
		"	public int value() default 0;\n"
		"}\n"
		"\n"
		"@SampleAnnotation(33)\n"
		"class Foo\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->annotationUses, L"Foo -> SampleAnnotation <6:2 6:17>"));
}

TEST_CASE("java parser finds usage of normal annotation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public @interface SampleAnnotation\n"
		"{\n"
		"	public int a() default 0;\n"
		"	public int b() default 0;\n"
		"}\n"
		"\n"
		"@SampleAnnotation()\n"
		"class Foo\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->annotationUses, L"Foo -> SampleAnnotation <7:2 7:17>"));
}

TEST_CASE("java parser finds usage of normal annotation member in initialization")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public @interface SampleAnnotation\n"
		"{\n"
		"	public int a() default 0;\n"
		"	public int b() default 0;\n"
		"}\n"
		"\n"
		"@SampleAnnotation(a = 9)\n"
		"class Foo\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->usages, L"Foo -> int SampleAnnotation.a <7:19 7:19>"));
}

TEST_CASE("java parser finds inheritance using extends keyword")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"}\n"
		"\n"
		"public class B extends A\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->inheritances, L"foo.B -> foo.A <6:24 6:24>"));
}

TEST_CASE("java parser finds inheritance using implements keyword")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"}\n"
		"\n"
		"public class B implements A\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->inheritances, L"foo.B -> foo.A <6:27 6:27>"));
}

TEST_CASE("java parser finds inheritance of anonymous class declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->inheritances, L"A.foo.anonymous class (input.java<10:3>) -> A.Base <9:16 9:19>"));
}

TEST_CASE("java parser finds usage of string for var type")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public void foo(){\n"
		"		var a = \"test\";\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.foo() -> java.lang.String <4:3 4:5>"));
}

TEST_CASE("java parser finds type parameter in signature of method")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A <T>\n"
		"{\n"
		"	public A<Void> foo(A<Void> a){\n"
		"		return a;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"A<java.lang.Void> A<T>.foo(A<java.lang.Void>) -> A<T> <3:9 3:9>"));
	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"A<java.lang.Void> A<T>.foo(A<java.lang.Void>) -> A<T> <3:21 3:21>"));
}

TEST_CASE("parser finds usage of type defined in base class")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class Foo {\n"
		"	public class Base {\n"
		"		public class X {\n"
		"		}\n"
		"	}\n"
		"	public class Derived extends Base {\n"
		"		public X x = null;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"Foo.Base.X Foo.Derived.x -> Foo.Base.X <7:10 7:10>"));
}

TEST_CASE("java parser finds correct location of qualified type usage")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public class B\n"
		"	{\n"
		"	}\n"
		"	void bar()\n"
		"	{\n"
		"		A.B b = new A.B();\n"
		"	}\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->typeUses, L"void A.bar() -> A.B <8:5 8:5>"));
}

TEST_CASE("java parser finds type argument of parameterized type")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A <T> {\n"
		"	T t;\n"
		"}\n"
		"public class B {\n"
		"	void foo() {\n"
		"		A<Void> a = null;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"A<T> -> java.lang.Void <6:5 6:8>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void B.foo() -> java.lang.Void <6:5 6:8>"));
}

TEST_CASE("java parser finds type argument of method invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	public static void foo() {\n"
		"	}\n"
		"\n"
		"	public static void bar() {\n"
		"		foo.X.<Void>foo();\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"static void foo.X.foo() -> java.lang.Void <8:10 8:13>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"static void foo.X.bar() -> java.lang.Void <8:10 8:13>"));
}

TEST_CASE("java parser finds type argument of super method invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"void foo.X.A.bar() -> java.lang.Void <15:11 15:14>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void foo.X.B.bar() -> java.lang.Void <15:11 15:14>"));
}

TEST_CASE("java parser finds type argument of constructor invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"foo.Bar.Bar() -> java.lang.Void <10:4 10:7>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"foo.Bar.Bar(int) -> java.lang.Void <10:4 10:7>"));
}

TEST_CASE("java parser finds type argument of super constructor invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"A.Base.Base() -> java.lang.Void <11:5 11:8>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"A.Derived.Derived() -> java.lang.Void <11:5 11:8>"));
}

TEST_CASE("java parser finds type argument of creation reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"A.B.B() -> java.lang.Void <14:24 14:27>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.foo() -> java.lang.Void <14:24 14:27>"));
}

TEST_CASE("java parser finds type argument of expression method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"static void A.bar() -> java.lang.Void <14:24 14:27>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.foo() -> java.lang.Void <14:24 14:27>"));
}

TEST_CASE("java parser finds type argument of super method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"void A.B.bar<T>() -> java.lang.Void <13:29 13:32>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.C.foo() -> java.lang.Void <13:29 13:32>"));
}

TEST_CASE("java parser finds type argument of type method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A {\n"
		"	\n"
		"	void foo() {\n"
		"		Functor method = int []::<Void>clone;\n"
		"	}\n"
		"}\n");

	// currently we cannot record the typeArguments of array type methods

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.foo() -> java.lang.Void <4:29 4:32>"));
}

TEST_CASE("java parser finds type argument of class instance creation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeArguments, L"A.B.B<T>() -> java.lang.Void <10:15 10:18>"));

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.B.bar() -> java.lang.Void <10:15 10:18>"));
}

TEST_CASE("java parser finds super method invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->calls, L"void foo.X.B.bar() -> void foo.X.A.bar() <15:10 15:12>"));
}

TEST_CASE("java parser finds constructor invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->calls, L"foo.Bar.Bar(int) -> foo.Bar.Bar() <10:3 10:6>"));
}

TEST_CASE("java parser finds super constructor invocation")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->calls, L"A.Derived.Derived() -> A.Base.Base() <11:4 11:8>"));
}

TEST_CASE("java parser finds invocation of method of anonymous class")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->calls,
		L"void Main.anonymous class (input.java<6:40>).foo() -> void Main.anonymous class "
		L"(input.java<6:40>).bar() <8:4 8:6>"));
}

TEST_CASE("java parser finds overridden method with same signature")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"class Main {\n"
		"	public interface Interfaze {\n"
		"		public void foo(int t);\n"
		"	}\n"
		"\n"
		"	public class C implements Interfaze {\n"
		"		public void foo(int t) {\n"
		"		}\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->overrides, L"void Main.C.foo(int) -> void Main.Interfaze.foo(int) <7:15 7:17>"));
}

TEST_CASE("java parser finds overridden method with generic signature")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"class Main <X> {\n"
		"	public interface Interfaze <T> {\n"
		"		public void foo(T t);\n"
		"	}\n"
		"\n"
		"	public class C implements Interfaze <X> {\n"
		"		public void foo(X t) {\n"
		"		}\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->overrides,
		L"void Main<X>.C.foo(Main<X>.X) -> void Main<X>.Interfaze<T>.foo(Main<X>.Interfaze<T>.T) "
		L"<7:15 7:17>"));
}

TEST_CASE("java parser finds method usage for creation reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->usages, L"void A.foo() -> A.B.B() <14:23 14:25>"));
}

TEST_CASE("java parser finds method usage for expression method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->usages, L"void A.foo() -> void A.B.bar() <17:23 17:25>"));
}

TEST_CASE("java parser finds method usage for super method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->usages, L"void A.C.foo() -> void A.B.bar() <13:28 13:30>"));
}

TEST_CASE("java parser finds no method usage for type method reference")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A {\n"
		"	void foo() {\n"
		"		Functor method = int []::clone;\n"
		"	}\n"
		"}\n");

	// finding method usage here may be implemented in the future.
	REQUIRE(client->usages.size() == 0);
}

TEST_CASE("java parser finds no usage of field within that fields declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	private int t;\n"
		"}\n");

	REQUIRE(client->usages.size() == 0);
}

TEST_CASE("java parser finds no usage of enum constant within that enum constants declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public enum X\n"
		"{\n"
		"	OPTION_0;\n"
		"}\n");

	REQUIRE(client->usages.size() == 0);
}

TEST_CASE("java parser finds usage of field with same name as method parameter")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	private int t;\n"
		"	public X(int t)\n"
		"	{\n"
		"		this.t = t;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->usages, L"foo.X.X(int) -> int foo.X.t <7:8 7:8>"));
}

TEST_CASE("java parser does not confuse method name with field name")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class X\n"
		"{\n"
		"	private int foo;\n"
		"	public void foo()\n"
		"	{\n"
		"		this.foo = 5;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->usages, L"void foo.X.foo() -> int foo.X.foo <7:8 7:10>"));
}

TEST_CASE("java parser finds assignment of method parameter")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	void bar(int i)\n"
		"	{\n"
		"		i = 0;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->localSymbols, L"foo.A.bar<0> <6:3 6:3>"));
}

TEST_CASE("java parser finds assignment of local variable")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"public class A\n"
		"{\n"
		"	void bar()\n"
		"	{\n"
		"		int i;\n"
		"		i = 0;\n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->localSymbols, L"foo.A.bar<0> <7:3 7:3>"));
}

TEST_CASE("java parser finds scope of class declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<2:1> <2:1 2:1>"));
	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<2:1> <3:1 3:1>"));
}

TEST_CASE("java parser finds scope of enum declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public enum A\n"
		"{\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<2:1> <2:1 2:1>"));
	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<2:1> <3:1 3:1>"));
}

TEST_CASE("java parser finds scope of constructor declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public A()\n"
		"	{\n"
		"	}\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<4:2> <4:2 4:2>"));
	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<4:2> <5:2 5:2>"));
}

TEST_CASE("java parser finds scope of method declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public void a()\n"
		"	{\n"
		"	}\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<4:2> <4:2 4:2>"));
	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<4:2> <5:2 5:2>"));
}

TEST_CASE("java parser finds scope of switch statement")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<6:3> <6:3 6:3>"));
	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<6:3> <9:3 9:3>"));
}

TEST_CASE("java parser finds scope of block statement")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public void a()\n"
		"	{\n"
		"		{\n"
		"		}\n"
		"	}\n"
		"}\n");

	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<5:3> <5:3 5:3>"));
	REQUIRE(
		utility::containsElement<std::wstring>(client->localSymbols, L"input.java<5:3> <6:3 6:3>"));
}

TEST_CASE("java parser finds scope of array initialization list")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	private int[] array = {1, 2};\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->localSymbols, L"input.java<3:24> <3:24 3:24>"));
	REQUIRE(utility::containsElement<std::wstring>(
		client->localSymbols, L"input.java<3:24> <3:29 3:29>"));
}

TEST_CASE("java parser finds scope of anonymous class declaration")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
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
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->localSymbols, L"input.java<10:3> <10:3 10:3>"));
	REQUIRE(utility::containsElement<std::wstring>(
		client->localSymbols, L"input.java<10:3> <11:3 11:3>"));
}

TEST_CASE("java parser finds usage of type parameter of class")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A <T>\n"
		"{\n"
		"	T t;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"A<T>.T A<T>.t -> A<T>.T <3:2 3:2>"));
}

TEST_CASE("java parser finds usage of type parameter of method")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A\n"
		"{\n"
		"	public <T> void foo(T t){};\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"void A.foo<T>(T) -> A.foo<T>.T <3:22 3:22>"));
}

TEST_CASE("java parser finds correct location of generic type usage")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A <T>\n"
		"{\n"
		"	A<Void> t;\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"A<java.lang.Void> A<T>.t -> A<T> <3:2 3:2>"));
}

TEST_CASE("java parser finds bound type of type parameter")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"public class A <T extends Void>\n"
		"{\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(
		client->typeUses, L"A<T>.T -> java.lang.Void <1:27 1:30>"));
}

TEST_CASE("java parsersupports java 12 switch expression")
{
	std::shared_ptr<TestIntermediateStorage> client = parseCode(
		"package foo;\n"
		"\n"
		"public class Foo {\n"
		"		enum FruitType {\n"
		"		PEAR, \n"
		"		APPLE\n"
		"	}\n"
		"	\n"
		"	public void foo() {\n"
		"		FruitType fruit = FruitType.APPLE; \n"
		"		int numberOfLetters2 = switch (fruit) {\n"
		"			case PEAR -> 4; \n"
		"			case APPLE -> 5; \n"
		"		}; \n"
		"	}\n"
		"}\n");

	REQUIRE(utility::containsElement<std::wstring>(client->usages, L"void foo.Foo.foo() -> foo.Foo.FruitType.PEAR <12:9 12:12>"));
}


#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
