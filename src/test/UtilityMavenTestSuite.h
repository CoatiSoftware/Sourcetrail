#include <cxxtest/TestSuite.h>

#include "FilePath.h"
#include "utilityMaven.h"
#include "utilityPathDetection.h"

class UtilityMavenTestSuite : public CxxTest::TestSuite
{
public:
	void test_maven_path_detector_is_working()
	{
		std::shared_ptr<PathDetector> mavenPathDetector = utility::getMavenExecutablePathDetector();
		TS_ASSERT(mavenPathDetector->isWorking());
	}

	void test_maven_wrapper_detects_source_directories_of_simple_projects()
	{
		std::shared_ptr<PathDetector> mavenPathDetector = utility::getMavenExecutablePathDetector();
		TS_ASSERT(!mavenPathDetector->getPaths().empty());

		if (!mavenPathDetector->getPaths().empty())
		{
			std::vector<FilePath> result = utility::mavenGetAllDirectoriesFromEffectivePom(
				mavenPathDetector->getPaths().front(), FilePath(L"data/UtilityMavenTestSuite/simple_maven_project"), FilePath(L"data/UtilityMavenTestSuite").makeAbsolute(), false
			);

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/simple_maven_project/src/main/java").makeAbsolute()
			));

			TS_ASSERT(!utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/simple_maven_project/src/test/java").makeAbsolute()
			));
		}
	}

	void test_maven_wrapper_detects_source_and_test_directories_of_simple_projects()
	{
		std::shared_ptr<PathDetector> mavenPathDetector = utility::getMavenExecutablePathDetector();
		TS_ASSERT(!mavenPathDetector->getPaths().empty());

		if (!mavenPathDetector->getPaths().empty())
		{
			std::vector<FilePath> result = utility::mavenGetAllDirectoriesFromEffectivePom(
				mavenPathDetector->getPaths().front(), FilePath(L"data/UtilityMavenTestSuite/simple_maven_project"), FilePath(L"data/UtilityMavenTestSuite").makeAbsolute(), true
			);

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/simple_maven_project/src/main/java").makeAbsolute()
			));

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/simple_maven_project/src/test/java").makeAbsolute()
			));
		}
	}

	void test_maven_wrapper_detects_source_directories_of_nested_modules()
	{
		std::shared_ptr<PathDetector> mavenPathDetector = utility::getMavenExecutablePathDetector();
		TS_ASSERT(!mavenPathDetector->getPaths().empty());

		if (!mavenPathDetector->getPaths().empty())
		{
			std::vector<FilePath> result = utility::mavenGetAllDirectoriesFromEffectivePom(
				mavenPathDetector->getPaths().front(), FilePath(L"data/UtilityMavenTestSuite/nested_maven_project"), FilePath(L"data/UtilityMavenTestSuite").makeAbsolute(), false
			);

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_1/src/main/java").makeAbsolute()
			));

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_2/src/main/java").makeAbsolute()
			));

			TS_ASSERT(!utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_1/src/test/java").makeAbsolute()
			));

			TS_ASSERT(!utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_2/src/test/java").makeAbsolute()
			));
		}
	}

	void test_maven_wrapper_detects_source_and_test_directories_of_nested_modules()
	{
		std::shared_ptr<PathDetector> mavenPathDetector = utility::getMavenExecutablePathDetector();
		TS_ASSERT(!mavenPathDetector->getPaths().empty());

		if (!mavenPathDetector->getPaths().empty())
		{
			std::vector<FilePath> result = utility::mavenGetAllDirectoriesFromEffectivePom(
				mavenPathDetector->getPaths().front(), FilePath(L"data/UtilityMavenTestSuite/nested_maven_project"), FilePath(L"data/UtilityMavenTestSuite").makeAbsolute(), true
			);

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_1/src/main/java").makeAbsolute()
			));

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_2/src/main/java").makeAbsolute()
			));

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_1/src/test/java").makeAbsolute()
			));

			TS_ASSERT(utility::containsElement<FilePath>(
				result, FilePath(L"data/UtilityMavenTestSuite/nested_maven_project/module_2/src/test/java").makeAbsolute()
			));
		}
	}
};
