#include "cxxtest/TestSuite.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "utility/file/FileSystem.h"

class FileSystemTestSuite : public CxxTest::TestSuite
{
public:
	void test_find_cpp_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".cpp");

		std::vector<std::string> cppFiles =
			FileSystem::getFileNamesFromDirectory(FilePath("data/FileSystemTestSuite"), extensions);

		TS_ASSERT_EQUALS(cppFiles.size(), 4);
		TS_ASSERT(isInVector(cppFiles, "data/FileSystemTestSuite/main.cpp"));
		TS_ASSERT(isInVector(cppFiles, "data/FileSystemTestSuite/Settings/sample.cpp"));
		TS_ASSERT(isInVector(cppFiles, "data/FileSystemTestSuite/src/main.cpp"));
		TS_ASSERT(isInVector(cppFiles, "data/FileSystemTestSuite/src/test.cpp"));
	}

	void test_find_h_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");

		std::vector<std::string> headerFiles =
			FileSystem::getFileNamesFromDirectory(FilePath("data/FileSystemTestSuite"), extensions);

		TS_ASSERT_EQUALS(headerFiles.size(), 3);
		TS_ASSERT(isInVector(headerFiles, "data/FileSystemTestSuite/tictactoe.h"));
		TS_ASSERT(isInVector(headerFiles, "data/FileSystemTestSuite/Settings/player.h"));
		TS_ASSERT(isInVector(headerFiles, "data/FileSystemTestSuite/src/test.h"));
	}

	void test_find_all_source_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".hpp");
		extensions.push_back(".cpp");

		std::vector<std::string> sourceFiles =
			FileSystem::getFileNamesFromDirectory(FilePath("data/FileSystemTestSuite"), extensions);

		TS_ASSERT_EQUALS(sourceFiles.size(), 8);
	}

	void test_find_file_infos()
	{
#ifndef _WIN32
		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".hpp");
		extensions.push_back(".cpp");

		std::vector<FilePath> directoryPaths;
		directoryPaths.push_back(FilePath("./data/FileSystemTestSuite/src"));

		std::vector<FileInfo> files = FileSystem::getFileInfosFromPaths(directoryPaths, extensions, false);

		TS_ASSERT_EQUALS(files.size(), 2);
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/test.cpp"));
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/test.h"));
#endif
	}

	void test_find_file_infos_with_symlinks()
	{
#ifndef _WIN32
		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".hpp");
		extensions.push_back(".cpp");

		std::vector<FilePath> directoryPaths;
		directoryPaths.push_back(FilePath("./data/FileSystemTestSuite/src"));

		std::vector<FileInfo> files = FileSystem::getFileInfosFromPaths(directoryPaths, extensions, true);

		TS_ASSERT_EQUALS(files.size(), 5);
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/Settings/player.h",
			"./data/FileSystemTestSuite/player.h"
		));
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/Settings/sample.cpp",
			"./data/FileSystemTestSuite/sample.cpp"
		));
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/main.cpp",
			"./data/FileSystemTestSuite/src/Settings/src/main.cpp"
		));
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/test.cpp",
			"./data/FileSystemTestSuite/src/Settings/src/test.cpp"
		));
		TS_ASSERT(isInFileInfos(files, "./data/FileSystemTestSuite/src/test.h",
			"./data/FileSystemTestSuite/src/Settings/src/test.h"
		));
#endif
	}

	void test_filesystem_finds_existing_files()
	{
		TS_ASSERT(FileSystem::exists(FilePath("data/FileSystemTestSuite")));
		TS_ASSERT(FileSystem::exists(FilePath("data/FileSystemTestSuite/tictactoe.h")));
	}

	void test_filesystem_does_not_find_non_existing_files()
	{
		TS_ASSERT(!FileSystem::exists(FilePath("data/FileSystemTestSuite/foo")));
		TS_ASSERT(!FileSystem::exists(FilePath("data/FileSystemTestSuite/blabla.h")));
	}

	void test_filesystem_extracts_filename()
	{
		TS_ASSERT_EQUALS(FileSystem::fileName("data/FileSystemTestSuite/tictactoe.h"), "tictactoe.h");
		TS_ASSERT_EQUALS(FileSystem::fileName("data/FileSystemTestSuite/Settings/player.h"), "player.h");
	}

	void test_filesystem_extracts_extension()
	{
		TS_ASSERT_EQUALS(FileSystem::extension("data/FileSystemTestSuite/tictactoe.h"), ".h");
		TS_ASSERT_EQUALS(FileSystem::extension("data/FileSystemTestSuite/tictactoe.cpp"), ".cpp");
	}

	void test_filesystem_extract_filepath_without_extension()
	{
		TS_ASSERT_EQUALS(FileSystem::filePathWithoutExtension("data/FileSystemTestSuite/tictactoe.h"), "data/FileSystemTestSuite/tictactoe");
		TS_ASSERT_EQUALS(FileSystem::filePathWithoutExtension("data/FileSystemTestSuite/tictactoe.cpp"), "data/FileSystemTestSuite/tictactoe");
	}

private:
	bool isInVector(const std::vector<std::string>& files, const std::string filename)
	{
		return std::end(files) != std::find(std::begin(files), std::end(files), filename);
	}

	bool isInFileInfos(const std::vector<FileInfo>& infos, const std::string filename)
	{
		for (const FileInfo& info : infos)
		{
			if (info.path.str() == filename)
			{
				return true;
			}
		}

		return false;
	}

	bool isInFileInfos(const std::vector<FileInfo>& infos, const std::string filename, const std::string filename2)
	{
		for (const FileInfo& info : infos)
		{
			if (info.path.str() == filename || info.path.str() == filename2)
			{
				return true;
			}
		}

		return false;
	}
};
