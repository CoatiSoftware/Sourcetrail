#include "cxxtest/TestSuite.h"

#include "utility/logging/logging.h"
#include "utility/solution/SolutionParserVisualStudio.h"

class VsSolutionParserTestSuite : public CxxTest::TestSuite
{
public:
	void test_load_solution()
	{
		SolutionParserVisualStudio parser;
		parser.openSolutionFile("data/vsSolutionParserTestSuite/Coati.sln");

		// TS_ASSERT_EQUALS(10186, parser.getSolutionCharCount());
	}

	void test_get_project_files()
	{
		SolutionParserVisualStudio parser;
		parser.openSolutionFile("data/vsSolutionParserTestSuite/Coati.sln");
		std::vector<std::string> projects = parser.getProjects();

		TS_ASSERT_EQUALS(9, projects.size());
		TS_ASSERT_EQUALS("ALL_BUILD.vcxproj", projects[0]);
		TS_ASSERT_EQUALS("Coati.vcxproj", projects[1]);
		TS_ASSERT_EQUALS("Coati_lib.vcxproj", projects[2]);
		TS_ASSERT_EQUALS("Coati_lib_gui.vcxproj", projects[3]);
		TS_ASSERT_EQUALS("Coati_lib_parser.vcxproj", projects[4]);
		TS_ASSERT_EQUALS("Coati_test.vcxproj", projects[5]);
		TS_ASSERT_EQUALS("Coati_trial.vcxproj", projects[6]);
		TS_ASSERT_EQUALS("ZERO_CHECK.vcxproj", projects[7]);
		TS_ASSERT_EQUALS("versionnumber.vcxproj", projects[8]);
	}

	void test_get_solution_name()
	{
		SolutionParserVisualStudio parser;
		parser.openSolutionFile("data/vsSolutionParserTestSuite/Coati.sln");
		std::string solutionName = parser.getSolutionName();

		TS_ASSERT_EQUALS("Coati", solutionName);
	}

	void test_get_include_paths()
	{
		SolutionParserVisualStudio parser;
		parser.openSolutionFile("data/vsSolutionParserTestSuite/Coati.sln");
		std::vector<std::string> includePaths = parser.getIncludePathsNonCanonical();

		TS_ASSERT_EQUALS(23, includePaths.size());
	}

	void test_get_()
	{
		SolutionParserVisualStudio parser;
		parser.openSolutionFile("data/vsSolutionParserTestSuite/Coati.sln");
		std::vector<std::string> projectItems = parser.getProjectItemsNonCanonical();

		TS_ASSERT_EQUALS(450, projectItems.size());
	}
};