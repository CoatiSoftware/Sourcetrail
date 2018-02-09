#include "cxxtest/TestSuite.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "utility/file/FileSystem.h"
#include "utility/utility.h"

class FileSystemTestSuite: public CxxTest::TestSuite
{
public:
	void test_find_cpp_files()
	{
		std::vector<std::wstring> cppFiles = utility::convert<FilePath, std::wstring>(
			FileSystem::getFilePathsFromDirectory(FilePath(L"data/FileSystemTestSuite"), { L".cpp" }),
			[](const FilePath& filePath){ return filePath.wstr(); }
		);

		TS_ASSERT_EQUALS(cppFiles.size(), 4);
		TS_ASSERT(utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/main.cpp"));
		TS_ASSERT(utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/Settings/sample.cpp"));
		TS_ASSERT(utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/src/main.cpp"));
		TS_ASSERT(utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/src/test.cpp"));
	}

	void test_find_h_files()
	{
		std::vector<std::wstring> headerFiles = utility::convert<FilePath, std::wstring>(
			FileSystem::getFilePathsFromDirectory(FilePath("data/FileSystemTestSuite"), { L".h" }),
			[](const FilePath& filePath){ return filePath.wstr(); }
		);

		TS_ASSERT_EQUALS(headerFiles.size(), 3);
		TS_ASSERT(utility::containsElement<std::wstring>(headerFiles, L"data/FileSystemTestSuite/tictactoe.h"));
		TS_ASSERT(utility::containsElement<std::wstring>(headerFiles, L"data/FileSystemTestSuite/Settings/player.h"));
		TS_ASSERT(utility::containsElement<std::wstring>(headerFiles, L"data/FileSystemTestSuite/src/test.h"));
	}

	void test_find_all_source_files()
	{
		std::vector<std::wstring> sourceFiles = utility::convert<FilePath, std::wstring>(
			FileSystem::getFilePathsFromDirectory(FilePath(L"data/FileSystemTestSuite"), { L".h", L".hpp", L".cpp" }),
			[](const FilePath& filePath){ return filePath.wstr(); }
		);

		TS_ASSERT_EQUALS(sourceFiles.size(), 8);
	}

	void test_find_file_infos()
	{
#ifndef _WIN32
		std::vector<FilePath> directoryPaths;
		directoryPaths.push_back(FilePath(L"./data/FileSystemTestSuite/src"));

		std::vector<FileInfo> files = FileSystem::getFileInfosFromPaths(directoryPaths, { L".h", L".hpp", L".cpp" }, false);

		TS_ASSERT_EQUALS(files.size(), 2);
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/test.cpp"));
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/test.h"));
#endif
	}

	void test_find_file_infos_with_symlinks()
	{
#ifndef _WIN32
		std::vector<FilePath> directoryPaths;
		directoryPaths.push_back(FilePath(L"./data/FileSystemTestSuite/src"));

		std::vector<FileInfo> files = FileSystem::getFileInfosFromPaths(directoryPaths, { L".h", L".hpp", L".cpp" }, true);

		TS_ASSERT_EQUALS(files.size(), 5);
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/Settings/player.h",
			L"./data/FileSystemTestSuite/player.h"
		));
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/Settings/sample.cpp",
			L"./data/FileSystemTestSuite/sample.cpp"
		));
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/main.cpp",
			L"./data/FileSystemTestSuite/src/Settings/src/main.cpp"
		));
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/test.cpp",
			L"./data/FileSystemTestSuite/src/Settings/src/test.cpp"
		));
		TS_ASSERT(isInFileInfos(files, L"./data/FileSystemTestSuite/src/test.h",
			L"./data/FileSystemTestSuite/src/Settings/src/test.h"
		));
#endif
	}

	void test_find_symlinked_directories()
	{
#ifndef _WIN32
		std::vector<FilePath> directoryPaths;
		directoryPaths.push_back(FilePath("./data/FileSystemTestSuite/src"));

		std::set<FilePath> dirs = FileSystem::getSymLinkedDirectories(directoryPaths);

		TS_ASSERT_EQUALS(dirs.size(), 2);
#endif
	}

private:
	bool isInFiles(const std::set<FilePath>& files, const FilePath& filename)
	{
		return std::end(files) != files.find(filename);
	}

	bool isInFileInfos(const std::vector<FileInfo>& infos, const std::wstring& filename)
	{
		for (const FileInfo& info : infos)
		{
			if (info.path.wstr() == filename)
			{
				return true;
			}
		}

		return false;
	}

	bool isInFileInfos(const std::vector<FileInfo>& infos, const std::wstring& filename, const std::wstring& filename2)
	{
		for (const FileInfo& info : infos)
		{
			if (info.path.wstr() == filename || info.path.wstr() == filename2)
			{
				return true;
			}
		}

		return false;
	}
};
