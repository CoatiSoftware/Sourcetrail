#include "cxxtest/TestSuite.h"

#include "utility/file/FileManager.h"

class FileManagerTestSuite : public CxxTest::TestSuite
{
public:
	void test_file_manager_is_created_empty()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back("./data/FileManagerTestSuite/src/");
		std::vector<FilePath> includePaths;
		includePaths.push_back("./data/FileManagerTestSuite/include/");
		std::vector<std::string> sourceExtensions;
		sourceExtensions.push_back(".cpp");
		sourceExtensions.push_back(".c");
		std::vector<std::string> includeExtensions;
		includeExtensions.push_back(".hpp");
		includeExtensions.push_back(".h");

		FileManager fm = FileManager(sourcePaths, includePaths, sourceExtensions, includeExtensions);

		TS_ASSERT_EQUALS(fm.getAddedFilePaths().size(), 0);
		TS_ASSERT_EQUALS(fm.getUpdatedFilePaths().size(), 0);
		TS_ASSERT_EQUALS(fm.getRemovedFilePaths().size(), 0);
	}

	void test_file_manager_has_added_file_paths_after_first_fetch()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back("./data/FileManagerTestSuite/src/");
		std::vector<FilePath> includePaths;
		includePaths.push_back("./data/FileManagerTestSuite/include/");
		std::vector<std::string> sourceExtensions;
		sourceExtensions.push_back(".cpp");
		sourceExtensions.push_back(".c");
		std::vector<std::string> includeExtensions;
		includeExtensions.push_back(".hpp");
		includeExtensions.push_back(".h");

		FileManager fm = FileManager(sourcePaths, includePaths, sourceExtensions, includeExtensions);
		fm.fetchFilePaths();

		TS_ASSERT_EQUALS(fm.getAddedFilePaths().size(), 4);
	}

	void test_file_manager_has_no_added_file_paths_after_second_fetch()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back("./data/FileManagerTestSuite/src/");
		std::vector<FilePath> includePaths;
		includePaths.push_back("./data/FileManagerTestSuite/include/");
		std::vector<std::string> sourceExtensions;
		sourceExtensions.push_back(".cpp");
		sourceExtensions.push_back(".c");
		std::vector<std::string> includeExtensions;
		includeExtensions.push_back(".hpp");
		includeExtensions.push_back(".h");

		FileManager fm = FileManager(sourcePaths, includePaths, sourceExtensions, includeExtensions);
		fm.fetchFilePaths();
		fm.fetchFilePaths();

		TS_ASSERT_EQUALS(fm.getAddedFilePaths().size(), 0);
	}

	void test_file_manager_has_updated_file_paths_after_second_fetch()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back("./data/FileManagerTestSuite/src/");
		std::vector<FilePath> includePaths;
		includePaths.push_back("./data/FileManagerTestSuite/include/");
		std::vector<std::string> sourceExtensions;
		sourceExtensions.push_back(".cpp");
		sourceExtensions.push_back(".c");
		std::vector<std::string> includeExtensions;
		includeExtensions.push_back(".hpp");
		includeExtensions.push_back(".h");

		FileManager fm = FileManager(sourcePaths, includePaths, sourceExtensions, includeExtensions);
		fm.fetchFilePaths();

		std::fstream fileStream;
		fileStream.open("./data/FileManagerTestSuite/include/c.h");
		fileStream << "update";
		fileStream.close();

		fm.fetchFilePaths();

		TS_ASSERT_EQUALS(fm.getAddedFilePaths().size(), 0);
		TS_ASSERT_EQUALS(fm.getUpdatedFilePaths().size(), 1);
		TS_ASSERT_EQUALS(fm.getRemovedFilePaths().size(), 0);
	}
};
