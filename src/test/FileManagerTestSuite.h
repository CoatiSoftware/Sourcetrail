#include "cxxtest/TestSuite.h"

#include "utility/file/FileManager.h"

class FileManagerTestSuite : public CxxTest::TestSuite
{
public:
	void test_file_manager_has_added_file_paths_after_first_fetch()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back(FilePath("./data/FileManagerTestSuite/src/"));
		sourcePaths.push_back(FilePath("./data/FileManagerTestSuite/include/"));
		std::vector<FilePath> headerPaths;
		std::vector<FilePath> excludePaths;
		std::vector<std::string> sourceExtensions;
		sourceExtensions.push_back(".cpp");
		sourceExtensions.push_back(".c");

		FileManager fm;
		fm.update(sourcePaths, excludePaths, sourceExtensions);
		std::set<FilePath> filePaths = fm.getAllSourceFilePaths();

		TS_ASSERT_EQUALS(filePaths.size(), 2);
	}
};
