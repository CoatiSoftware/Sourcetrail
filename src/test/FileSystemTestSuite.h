#include "cxxtest/TestSuite.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "utility/FileSystem.h"

class FileSystemTestSuite : public CxxTest::TestSuite
{
public:
	void test_find_cpp_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".cpp");

		std::vector<std::string> cppFiles =
			FileSystem::getFileNamesFromDirectory("data/FileSystemTestSuite", extensions);

		TS_ASSERT_EQUALS(cppFiles.size(), 2);
		TS_ASSERT(isInVector(cppFiles, "data/FileSystemTestSuite/main.cpp"));
		TS_ASSERT(isInVector(cppFiles, "data/FileSystemTestSuite/Settings/sample.cpp"));
	}

	void test_find_h_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");

		std::vector<std::string> headerFiles =
			FileSystem::getFileNamesFromDirectory("data/FileSystemTestSuite", extensions);

		TS_ASSERT_EQUALS(headerFiles.size(), 2);
		TS_ASSERT(isInVector(headerFiles, "data/FileSystemTestSuite/tictactoe.h"));
		TS_ASSERT(isInVector(headerFiles, "data/FileSystemTestSuite/Settings/player.h"));
	}

	void test_find_all_source_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".hpp");
		extensions.push_back(".cpp");

		std::vector<std::string> sourceFiles =
			FileSystem::getFileNamesFromDirectory("data/FileSystemTestSuite", extensions);

		TS_ASSERT_EQUALS(sourceFiles.size(), 5);
	}

	void test_find_updated_source_files()
	{
		std::string timeString = FileSystem::getTimeStringNow();

		std::fstream fileStream;
		fileStream.open("./data/FileSystemTestSuite/update.c");
		fileStream << "update";
		fileStream.close();

		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".c");
		extensions.push_back(".hpp");
		extensions.push_back(".cpp");

		std::vector<std::string> sourceFiles =
			FileSystem::getFileNamesFromDirectoryUpdatedAfter("data/FileSystemTestSuite", extensions, timeString);

		TS_ASSERT_EQUALS(sourceFiles.size(), 1);
		TS_ASSERT_EQUALS(sourceFiles[0], "data/FileSystemTestSuite/update.c");
	}

	void test_filesystem_finds_existing_files()
	{
		TS_ASSERT(FileSystem::exists("data/FileSystemTestSuite"));
		TS_ASSERT(FileSystem::exists("data/FileSystemTestSuite/tictactoe.h"));
	}

	void test_filesystem_does_not_find_non_existing_files()
	{
		TS_ASSERT(!FileSystem::exists("data/FileSystemTestSuite/foo"));
		TS_ASSERT(!FileSystem::exists("data/FileSystemTestSuite/blabla.h"));
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
};
