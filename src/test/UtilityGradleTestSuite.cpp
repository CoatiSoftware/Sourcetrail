#include "catch.hpp"

#include "language_packages.h"

#if BUILD_JAVA_LANGUAGE_PACKAGE
//#	ifdef WIN32
//
//#		include "FilePath.h"
//#		include "utility.h"
//#		include "utilityGradle.h"
//
// TEST_CASE("gradle wrapper detects source directories of simple projects")
//{
//	std::vector<FilePath> result = utility::gradleGetAllSourceDirectories(
//		FilePath(L"data/UtilityGradleTestSuite/simple_gradle_project"), false);
//
//	REQUIRE(utility::containsElement<FilePath>(
//		result,
//		FilePath(L"data/UtilityGradleTestSuite/simple_gradle_project/src/main/java").makeAbsolute()));
//
//	REQUIRE(!utility::containsElement<FilePath>(
//		result,
//		FilePath(L"data/UtilityGradleTestSuite/simple_gradle_project/src/test/java").makeAbsolute()));
//}
//
// TEST_CASE("gradle wrapper detects source and test directories of simple projects")
//{
//	std::vector<FilePath> result = utility::gradleGetAllSourceDirectories(
//		FilePath(L"data/UtilityGradleTestSuite/simple_gradle_project"), true);
//
//	REQUIRE(utility::containsElement<FilePath>(
//		result,
//		FilePath(L"data/UtilityGradleTestSuite/simple_gradle_project/src/main/java").makeAbsolute()));
//
//	REQUIRE(utility::containsElement<FilePath>(
//		result,
//		FilePath(L"data/UtilityGradleTestSuite/simple_gradle_project/src/test/java").makeAbsolute()));
//}
//
//#	endif	  // WIN32
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
