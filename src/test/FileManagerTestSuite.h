#include "cxxtest/TestSuite.h"

#include "utility/file/FileManager.h"
#include "utility/file/FilePath.h"
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
		std::vector<FilePath> excludePaths;
		std::wstring specialExtension = L".";
#ifdef _WIN32
		const wchar_t specialCharacter(252);
		specialExtension += specialCharacter;
#else
		std::wstring c;
		specialExtension += wchar_t(117);
		specialExtension += wchar_t(776);
#endif
		std::vector<std::wstring> sourceExtensions = { L".cpp", L".c", specialExtension };

		FileManager fm;
		fm.update(sourcePaths, excludePaths, sourceExtensions);
		std::vector<FilePath> filePaths = utility::toVector(fm.getAllSourceFilePaths());

		TS_ASSERT_EQUALS(filePaths.size(), 3);
		TS_ASSERT(utility::containsElement<FilePath>(filePaths, FilePath(L"./data/FileManagerTestSuite/src/a.cpp")));
		TS_ASSERT(utility::containsElement<FilePath>(filePaths, FilePath(L"./data/FileManagerTestSuite/src/d.c")));
		TS_ASSERT(utility::containsElement<FilePath>(filePaths, FilePath(L"./data/FileManagerTestSuite/src/e" + specialExtension)));
	}
};
