#include <cxxtest/TestSuite.h>

#include "ProjectSettings.h"

class ProjectSettingsTestSuite : public CxxTest::TestSuite
{
public:
	void test_loading_projectSettings_from_file()
	{		
		TS_ASSERT(ProjectSettings::getInstance()->load("data/test_ProjectSettings.xml"));
	}

	void test_load_sourcepath_from_file()
	{
		ProjectSettings::getInstance()->load("data/test_ProjectSettings.xml");
		TS_ASSERT_EQUALS(ProjectSettings::getInstance()->getSourcePath(), "data");
	}
};
