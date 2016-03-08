#include "cxxtest/TestSuite.h"

#include "utility/file/FileManager.h"

class FileManagerTestSuite : public CxxTest::TestSuite
{
public:
	void test_file_manager_is_created_empty()
	{
		FileManager fm = FileManager();

		TS_ASSERT_EQUALS(fm.getAddedFilePaths().size(), 0);
		TS_ASSERT_EQUALS(fm.getUpdatedFilePaths().size(), 0);
		TS_ASSERT_EQUALS(fm.getRemovedFilePaths().size(), 0);
	}

	void test_file_manager_has_added_file_paths_after_first_fetch()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back("./data/FileManagerTestSuite/src/");
		sourcePaths.push_back("./data/FileManagerTestSuite/include/");
		std::vector<FilePath> headerPaths;
		std::vector<std::string> sourceExtensions;
		sourceExtensions.push_back(".cpp");
		sourceExtensions.push_back(".c");
		std::vector<std::string> includeExtensions;
		includeExtensions.push_back(".hpp");
		includeExtensions.push_back(".h");

		FileManager fm;
		fm.setPaths(sourcePaths, headerPaths, sourceExtensions, includeExtensions);
		fm.fetchFilePaths(std::vector<FileInfo>());

		TS_ASSERT_EQUALS(fm.getAddedFilePaths().size(), 4);
	}
};
