#include "cxxtest/TestSuite.h"

#include <fstream>
#include <iostream>


#include "data/indexer/IndexerCommandJava.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "data/parser/java/JavaParser.h"
#include "helper/DumpParserClient.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileRegister.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityJava.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityString.h"

class JavaIndexSampleProjectsTestSuite : public CxxTest::TestSuite
{
public:
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

	void test_index_javasymbolsolver_0_6_0_project()
	{
		const bool updateExpectedOutput = false;

		const std::vector<FilePath>& classpath = { 
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/guava-21.0.jar").makeAbsolute(),
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/javaparser-core-3.3.0.jar").makeAbsolute(),
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/javaslang-2.0.3.jar").makeAbsolute(),
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/javassist-3.19.0-GA.jar").makeAbsolute(),
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/src/java-symbol-solver-core").makeAbsolute(),
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/src/java-symbol-solver-logic").makeAbsolute(),
			FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/src/java-symbol-solver-model").makeAbsolute()
		};

		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/SourceFileInfoExtractor.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/core/resolution/Context.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/core/resolution/ContextHelper.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/declarations/common/MethodDeclarationCommonLogic.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparser/Navigator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparser/package-info.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/DefaultVisitorAdapter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/JavaParserFacade.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/JavaParserFactory.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/LambdaArgumentTypePlaceholder.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/package-info.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/TypeExtractor.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/UnsolvedSymbolException.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/AbstractJavaParserContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/AbstractMethodLikeDeclarationContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/AnonymousClassDeclarationContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/CatchClauseContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/ClassOrInterfaceDeclarationContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/CompilationUnitContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/ConstructorContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/ContextHelper.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/EnumDeclarationContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/FieldAccessContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/ForechStatementContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/ForStatementContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/JavaParserTypeDeclarationAdapter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/LambdaExprContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/MethodCallExprContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/MethodContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/StatementContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/SwitchEntryContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/contexts/TryWithResourceContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/DefaultConstructorDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/Helper.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserAnnotationDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserAnonymousClassDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserClassDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserConstructorDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserEnumConstantDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserEnumDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserFieldDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserInterfaceDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserMethodDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserParameterDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserSymbolDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserTypeAdapter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserTypeParameter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarations/JavaParserTypeVariableDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarators/AbstractSymbolDeclarator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarators/FieldSymbolDeclarator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarators/NoSymbolDeclarator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarators/ParameterSymbolDeclarator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/declarators/VariableSymbolDeclarator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistClassDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistConstructorDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistEnumDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistFactory.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistFieldDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistInterfaceDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistMethodDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistParameterDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistTypeDeclarationAdapter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistTypeParameter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/JavassistUtils.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/package-info.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/model/typesystem/LazyType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/model/typesystem/ReferenceTypeImpl.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/MyObjectProvider.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/package-info.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionClassAdapter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionClassDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionConstructorDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionEnumDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionFactory.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionFieldDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionInterfaceDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionMethodDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionMethodResolutionLogic.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionParameterDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/ReflectionTypeParameter.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/comparators/ClassComparator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/comparators/MethodComparator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/comparators/ParameterComparator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/ConstructorResolutionLogic.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/MethodResolutionLogic.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/SymbolDeclarator.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/SymbolSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/typesolvers/CombinedTypeSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/typesolvers/JarTypeSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/typesolvers/JavaParserTypeSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/typesolvers/MemoryTypeSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/typesolvers/ReflectionTypeSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/AbstractClassDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/AbstractTypeDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/ConfilictingGenericTypesException.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/FunctionalInterfaceLogic.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/InferenceContext.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/InferenceVariableType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/ObjectProvider.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/AccessLevel.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/AnnotationDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/ClassDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/ConstructorDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/Declaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/EnumDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/FieldDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/HasAccessLevel.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/InterfaceDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/MethodAmbiguityException.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/MethodDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/MethodLikeDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/ParameterDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/ReferenceTypeDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/TypeDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/TypeParameterDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/TypeParametrizable.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/ValueDeclaration.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/methods/MethodUsage.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/SymbolReference.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/TypeSolver.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/UnsolvedSymbolException.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/Value.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/ArrayType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/LambdaConstraintType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/NullType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/PrimitiveType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/ReferenceType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/Type.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/TypeTransformer.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/TypeVariable.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/VoidType.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/Wildcard.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/parametrization/TypeParametersMap.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/parametrization/TypeParameterValueProvider.java"), classpath, updateExpectedOutput);
		processSourceFile("JavaSymbolSolver060", FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/parametrization/TypeParametrized.java"), classpath, updateExpectedOutput);
	}

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

			JavaEnvironmentFactory::createInstance(
				classPath,
				errorString
			);
		}
	}

	void processSourceFile(const std::string& projectName, const FilePath& sourceFilePath, const std::vector<FilePath>& classpath, const bool updateExpectedOutput)
	{
		const FilePath projectDataRoot = FilePath("data/JavaIndexSampleProjectsTestSuite/" + projectName);
		const FilePath projectDataSrcRoot = projectDataRoot.getConcatenated(L"src");
		const FilePath projectDataExpectedOutputRoot = projectDataRoot.getConcatenated(L"expected_output");

		std::shared_ptr<TextAccess> output = parseCode(projectDataSrcRoot.getConcatenated(sourceFilePath), projectDataSrcRoot, classpath);

		const FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.getConcatenated(utility::replace(sourceFilePath.withoutExtension().wstr() + L".txt", L"/", L"_"));
		if (updateExpectedOutput || !expectedOutputFilePath.exists())
		{
			std::ofstream expectedOutputFile;
			expectedOutputFile.open(expectedOutputFilePath.str());
			expectedOutputFile << output->getText();
			expectedOutputFile.close();
		}
		else
		{
			std::shared_ptr<TextAccess> expectedOutput = TextAccess::createFromFile(expectedOutputFilePath);
			TSM_ASSERT_EQUALS("Output does not match the expected line count for file " + sourceFilePath.str() + " in project " + projectName, expectedOutput->getLineCount(), output->getLineCount());
			if (expectedOutput->getLineCount() == output->getLineCount())
			{
				for (size_t i = 1; i <= expectedOutput->getLineCount(); i++)
				{
					TS_ASSERT_EQUALS(expectedOutput->getLine(i), output->getLine(i));
				}
			}
		}
	}

	std::shared_ptr<TextAccess> parseCode(const FilePath& sourceFilePath, const FilePath& projectDataSrcRoot, const std::vector<FilePath>& classpath)
	{
		std::set<FilePath> indexedPaths = { projectDataSrcRoot };
		std::set<FilePathFilter> excludeFilters = { };

		std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
			sourceFilePath,
			indexedPaths,
			excludeFilters
		);

		std::shared_ptr<DumpParserClient> parserClient = std::make_shared<DumpParserClient>();

		JavaParser parser(parserClient, fileRegister);
		std::shared_ptr<IndexerCommandJava> command = std::make_shared<IndexerCommandJava>(sourceFilePath, indexedPaths, excludeFilters, "8", classpath);

		parser.buildIndex(command);

		return TextAccess::createFromString(utility::encodeToUtf8(parserClient->m_lines));
	}
};