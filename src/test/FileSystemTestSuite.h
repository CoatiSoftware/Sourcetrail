#include "cxxtest/TestSuite.h"

#include <algorithm>
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

		std::vector<std::string> cppFiles = FileSystem::getSourceFilesFromDirectory("data/src",extensions);

		TS_ASSERT_EQUALS(cppFiles.size(), 2)
		TS_ASSERT(isInVector(cppFiles, "data/src/main.cpp"))
		TS_ASSERT(isInVector(cppFiles, "data/src/Settings/sample.cpp"))
	}

	void test_find_h_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");

		std::vector<std::string> headerFiles = FileSystem::getSourceFilesFromDirectory("data/src",extensions);

		TS_ASSERT_EQUALS(headerFiles.size(), 2)
		TS_ASSERT(isInVector(headerFiles, "data/src/tictactoe.h"))
		TS_ASSERT(isInVector(headerFiles, "data/src/Settings/player.h"))
	}

	void test_find_all_source_files()
	{
		std::vector<std::string> extensions;
		extensions.push_back(".h");
		extensions.push_back(".hpp");
		extensions.push_back(".cpp");

		std::vector<std::string> sourceFiles = FileSystem::getSourceFilesFromDirectory("data/src",extensions);

		TS_ASSERT_EQUALS(sourceFiles.size(), 5)
	}

	void test_filesystem_finds_existing_files()
	{
		TS_ASSERT(FileSystem::exists("data/src"))
		TS_ASSERT(FileSystem::exists("data/src/tictactoe.h"));
	}

	void test_filesystem_does_not_find_non_existing_files()
	{
		TS_ASSERT(!FileSystem::exists("data/foo"));
		TS_ASSERT(!FileSystem::exists("data/src/blabla.h"));
	}

private:
	bool isInVector(const std::vector<std::string>& files, const std::string filename)
	{
		return std::end(files) != std::find(std::begin(files), std::end(files), filename);
	}
};
