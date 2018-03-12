#include "cxxtest/TestSuite.h"

#include "utility/file/FileManager.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/utility.h"

class FileManagerTestSuite : public CxxTest::TestSuite
{
public:
	void test_file_manager_has_added_file_paths_after_first_fetch()
	{
		std::vector<FilePath> sourcePaths;
		sourcePaths.push_back(FilePath(L"./data/FileManagerTestSuite/src/"));
		sourcePaths.push_back(FilePath(L"./data/FileManagerTestSuite/include/"));
		std::vector<FilePath> headerPaths;
		std::vector<FilePathFilter> excludeFilters;

		std::vector<FilePath> filePaths = FileSystem::getFilePathsFromDirectory(FilePath(L"./data/FileManagerTestSuite/src/"));
		TS_ASSERT_EQUALS(filePaths.size(), 3);

		std::vector<std::wstring> sourceExtensions;
		for (FilePath p : filePaths)
		{
			sourceExtensions.push_back(p.extension());
		}
		TS_ASSERT_EQUALS(sourceExtensions.size(), 3);

		FileManager fm;
		fm.update(sourcePaths, excludeFilters, sourceExtensions);
		std::vector<FilePath> foundSourcePaths = utility::toVector(fm.getAllSourceFilePaths());

		TS_ASSERT_EQUALS(foundSourcePaths.size(), 3);
		TS_ASSERT(utility::containsElement<FilePath>(foundSourcePaths, filePaths[0]));
		TS_ASSERT(utility::containsElement<FilePath>(foundSourcePaths, filePaths[1]));
		TS_ASSERT(utility::containsElement<FilePath>(foundSourcePaths, filePaths[2]));
	}
};
