#include "catch.hpp"

#include "language_packages.h"

#if BUILD_JAVA_LANGUAGE_PACKAGE

#	include <fstream>
#	include <iostream>

#	include "ApplicationSettings.h"
#	include "FileRegister.h"
#	include "IndexerCommandJava.h"
#	include "JavaEnvironmentFactory.h"
#	include "JavaParser.h"
#	include "ParserClientImpl.h"
#	include "TestStorage.h"
#	include "TextAccess.h"
#	include "TimeStamp.h"
#	include "utility.h"
#	include "utilityJava.h"
#	include "utilityPathDetection.h"
#	include "utilityString.h"

#	define REQUIRE_MESSAGE(msg, cond)                                                             \
		do                                                                                         \
		{                                                                                          \
			INFO(msg);                                                                             \
			REQUIRE(cond);                                                                         \
		} while ((void)0, 0)

namespace
{
const bool updateExpectedOutput = false;
const bool trackTime = true;
size_t duration;

void setupJavaEnvironmentFactory()
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
	}
}

std::shared_ptr<TextAccess> parseCode(
	const FilePath& sourceFilePath,
	const FilePath& projectDataSrcRoot,
	const std::vector<FilePath>& classpath)
{
	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
	JavaParser parser(
		std::make_shared<ParserClientImpl>(storage.get()), std::make_shared<IndexerStateInfo>());
	std::shared_ptr<IndexerCommandJava> command = std::make_shared<IndexerCommandJava>(
		sourceFilePath, L"12", classpath);

	TimeStamp startTime = TimeStamp::now();
	parser.buildIndex(command);
	duration += TimeStamp::now().deltaMS(startTime);

	return TextAccess::createFromLines(TestStorage::create(storage)->m_lines);
}

void processSourceFile(
	const std::string& projectName,
	const FilePath& sourceFilePath,
	const std::vector<FilePath>& classpath)
{
	const FilePath projectDataRoot = FilePath("data/JavaIndexSampleProjectsTestSuite/" + projectName);
	const FilePath projectDataSrcRoot = projectDataRoot.getConcatenated(L"src");
	const FilePath projectDataExpectedOutputRoot = projectDataRoot.getConcatenated(
		L"expected_output");

	std::shared_ptr<TextAccess> output = parseCode(
		projectDataSrcRoot.getConcatenated(sourceFilePath), projectDataSrcRoot, classpath);

	const FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.getConcatenated(
		utility::replace(sourceFilePath.withoutExtension().wstr() + L".txt", L"/", L"_"));
	if (updateExpectedOutput || !expectedOutputFilePath.exists())
	{
		std::ofstream expectedOutputFile;
		expectedOutputFile.open(expectedOutputFilePath.str());
		expectedOutputFile << output->getText();
		expectedOutputFile.close();
	}
	else
	{
		std::shared_ptr<TextAccess> expectedOutput = TextAccess::createFromFile(
			expectedOutputFilePath);
		REQUIRE_MESSAGE(
			("Output does not match the expected line count for file " + sourceFilePath.str() +
			 " in project " + projectName)
				.c_str(),
			expectedOutput->getLineCount() == output->getLineCount());
		if (expectedOutput->getLineCount() == output->getLineCount())
		{
			for (unsigned int i = 1; i <= expectedOutput->getLineCount(); i++)
			{
				REQUIRE(expectedOutput->getLine(i) == output->getLine(i));
			}
		}
	}
}

void processSourceFiles(
	const std::string& projectName,
	const std::vector<FilePath>& sourceFilePaths,
	const std::vector<FilePath>& classpath)
{
	duration = 0;
	for (const FilePath& filePath: sourceFilePaths)
	{
		processSourceFile(projectName, filePath, classpath);
	}
	if (trackTime)
	{
		const FilePath projectDataRoot =
			FilePath("data/JavaIndexSampleProjectsTestSuite/" + projectName).makeAbsolute();

		std::ofstream outfile;
		outfile.open(
			FilePath(projectDataRoot.str() + "/" + projectName + ".timing").str(),
			std::ios_base::app);
		outfile << TimeStamp::now().toString() << " - " << duration << " ms\n";
		outfile.close();
	}
}
}	 // namespace

TEST_CASE("java sample parser can setup environment factory")
{
	std::vector<FilePath> javaPaths = utility::getJavaRuntimePathDetector()->getPaths();
	if (!javaPaths.empty())
	{
		ApplicationSettings::getInstance()->setJavaPath(javaPaths[0]);
	}

	setupJavaEnvironmentFactory();

	// if this one fails, maybe your java_path in the test settings is wrong.
	REQUIRE(JavaEnvironmentFactory::getInstance().use_count() >= 1);
}

TEST_CASE("index javasymbolsolver 0 6 0 project")
{
	const std::vector<FilePath>& classpath = {
		FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/guava-21.0.jar")
			.makeAbsolute(),
		FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/"
				 L"javaparser-core-3.3.0.jar")
			.makeAbsolute(),
		FilePath(
			L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/javaslang-2.0.3.jar")
			.makeAbsolute(),
		FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/lib/"
				 L"javassist-3.19.0-GA.jar")
			.makeAbsolute(),
		FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/src/"
				 L"java-symbol-solver-core")
			.makeAbsolute(),
		FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/src/"
				 L"java-symbol-solver-logic")
			.makeAbsolute(),
		FilePath(L"data/JavaIndexSampleProjectsTestSuite/JavaSymbolSolver060/src/"
				 L"java-symbol-solver-model")
			.makeAbsolute()};

	processSourceFiles(
		"JavaSymbolSolver060",
		{FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/"
				  L"SourceFileInfoExtractor.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/core/resolution/"
				  L"Context.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/core/resolution/"
				  L"ContextHelper.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/declarations/common/"
				  L"MethodDeclarationCommonLogic.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparser/"
				  L"Navigator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparser/"
				  L"package-info.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"DefaultVisitorAdapter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"JavaParserFacade.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"JavaParserFactory.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"LambdaArgumentTypePlaceholder.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"package-info.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"TypeExtractor.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"UnsolvedSymbolException.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/AbstractJavaParserContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/AbstractMethodLikeDeclarationContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/AnonymousClassDeclarationContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/CatchClauseContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/ClassOrInterfaceDeclarationContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/CompilationUnitContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/ConstructorContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/ContextHelper.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/EnumDeclarationContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/FieldAccessContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/ForechStatementContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/ForStatementContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/JavaParserTypeDeclarationAdapter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/LambdaExprContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/MethodCallExprContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/MethodContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/StatementContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/SwitchEntryContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"contexts/TryWithResourceContext.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/DefaultConstructorDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/Helper.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserAnnotationDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserAnonymousClassDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserClassDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserConstructorDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserEnumConstantDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserEnumDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserFieldDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserInterfaceDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserMethodDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserParameterDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserSymbolDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserTypeAdapter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserTypeParameter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarations/JavaParserTypeVariableDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarators/AbstractSymbolDeclarator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarators/FieldSymbolDeclarator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarators/NoSymbolDeclarator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarators/ParameterSymbolDeclarator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javaparsermodel/"
				  L"declarators/VariableSymbolDeclarator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistClassDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistConstructorDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistEnumDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistFactory.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistFieldDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistInterfaceDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistMethodDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistParameterDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistTypeDeclarationAdapter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistTypeParameter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"JavassistUtils.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/javassistmodel/"
				  L"package-info.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"LazyType.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"ReferenceTypeImpl.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"MyObjectProvider.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"package-info.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionClassAdapter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionClassDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionConstructorDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionEnumDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionFactory.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionFieldDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionInterfaceDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionMethodDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionMethodResolutionLogic.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionParameterDeclaration.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"ReflectionTypeParameter.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"comparators/ClassComparator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"comparators/MethodComparator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/reflectionmodel/"
				  L"comparators/ParameterComparator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"ConstructorResolutionLogic.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"MethodResolutionLogic.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"SymbolDeclarator.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"SymbolSolver.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"typesolvers/CombinedTypeSolver.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"typesolvers/JarTypeSolver.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"typesolvers/JavaParserTypeSolver.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"typesolvers/MemoryTypeSolver.java"),
		 FilePath(L"java-symbol-solver-core/com/github/javaparser/symbolsolver/resolution/"
				  L"typesolvers/ReflectionTypeSolver.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"AbstractClassDeclaration.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"AbstractTypeDeclaration.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"ConfilictingGenericTypesException.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"FunctionalInterfaceLogic.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"InferenceContext.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"InferenceVariableType.java"),
		 FilePath(L"java-symbol-solver-logic/com/github/javaparser/symbolsolver/logic/"
				  L"ObjectProvider.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"AccessLevel.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"AnnotationDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"ClassDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"ConstructorDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"Declaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"EnumDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"FieldDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"HasAccessLevel.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"InterfaceDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"MethodAmbiguityException.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"MethodDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"MethodLikeDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"ParameterDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"ReferenceTypeDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"TypeDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"TypeParameterDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"TypeParametrizable.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/declarations/"
				  L"ValueDeclaration.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/methods/"
				  L"MethodUsage.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/"
				  L"SymbolReference.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/"
				  L"TypeSolver.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/"
				  L"UnsolvedSymbolException.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/resolution/"
				  L"Value.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"ArrayType.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"LambdaConstraintType.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"NullType.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"PrimitiveType.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"ReferenceType.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"Type.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"TypeTransformer.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"TypeVariable.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"VoidType.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"Wildcard.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"parametrization/TypeParametersMap.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"parametrization/TypeParameterValueProvider.java"),
		 FilePath(L"java-symbol-solver-model/com/github/javaparser/symbolsolver/model/typesystem/"
				  L"parametrization/TypeParametrized.java")},
		classpath);
}

#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
