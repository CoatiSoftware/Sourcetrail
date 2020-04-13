#include "catch.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "../lib/utility/file/FileSystem.h"
#include "../lib/utility/utility.h"

namespace
{
bool isInFiles(const std::set<FilePath>& files, const FilePath& filename)
{
	return std::end(files) != files.find(filename);
}

bool isInFileInfos(const std::vector<FileInfo>& infos, const std::wstring& filename)
{
	for (const FileInfo& info: infos)
	{
		if (info.path.wstr() == filename)
		{
			return true;
		}
	}

	return false;
}

bool isInFileInfos(
	const std::vector<FileInfo>& infos, const std::wstring& filename, const std::wstring& filename2)
{
	for (const FileInfo& info: infos)
	{
		if (info.path.wstr() == filename || info.path.wstr() == filename2)
		{
			return true;
		}
	}

	return false;
}
}	 // namespace

TEST_CASE("find cpp files")
{
	std::vector<std::wstring> cppFiles = utility::convert<FilePath, std::wstring>(
		FileSystem::getFilePathsFromDirectory(FilePath(L"data/FileSystemTestSuite"), {L".cpp"}),
		[](const FilePath& filePath) { return filePath.wstr(); });

	REQUIRE(cppFiles.size() == 4);
	REQUIRE(utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/main.cpp"));
	REQUIRE(utility::containsElement<std::wstring>(
		cppFiles, L"data/FileSystemTestSuite/Settings/sample.cpp"));
	REQUIRE(
		utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/src/main.cpp"));
	REQUIRE(
		utility::containsElement<std::wstring>(cppFiles, L"data/FileSystemTestSuite/src/test.cpp"));
}

TEST_CASE("find h files")
{
	std::vector<std::wstring> headerFiles = utility::convert<FilePath, std::wstring>(
		FileSystem::getFilePathsFromDirectory(FilePath(L"data/FileSystemTestSuite"), {L".h"}),
		[](const FilePath& filePath) { return filePath.wstr(); });

	REQUIRE(headerFiles.size() == 3);
	REQUIRE(utility::containsElement<std::wstring>(
		headerFiles, L"data/FileSystemTestSuite/tictactoe.h"));
	REQUIRE(utility::containsElement<std::wstring>(
		headerFiles, L"data/FileSystemTestSuite/Settings/player.h"));
	REQUIRE(utility::containsElement<std::wstring>(
		headerFiles, L"data/FileSystemTestSuite/src/test.h"));
}

TEST_CASE("find all source files")
{
	std::vector<std::wstring> sourceFiles = utility::convert<FilePath, std::wstring>(
		FileSystem::getFilePathsFromDirectory(
			FilePath(L"data/FileSystemTestSuite"), {L".h", L".hpp", L".cpp"}),
		[](const FilePath& filePath) { return filePath.wstr(); });

	REQUIRE(sourceFiles.size() == 8);
}

TEST_CASE("find file infos")
{
#ifndef _WIN32
	std::vector<FilePath> directoryPaths;
	directoryPaths.push_back(FilePath(L"./data/FileSystemTestSuite/src"));

	std::vector<FileInfo> files = FileSystem::getFileInfosFromPaths(
		directoryPaths, {L".h", L".hpp", L".cpp"}, false);

	REQUIRE(files.size() == 2);
	REQUIRE(isInFileInfos(files, L"./data/FileSystemTestSuite/src/test.cpp"));
	REQUIRE(isInFileInfos(files, L"./data/FileSystemTestSuite/src/test.h"));
#endif
}

TEST_CASE("find file infos with symlinks")
{
#ifndef _WIN32
	std::vector<FilePath> directoryPaths;
	directoryPaths.push_back(FilePath(L"./data/FileSystemTestSuite/src"));

	std::vector<FileInfo> files = FileSystem::getFileInfosFromPaths(
		directoryPaths, {L".h", L".hpp", L".cpp"}, true);

	REQUIRE(files.size() == 5);
	REQUIRE(isInFileInfos(
		files,
		L"./data/FileSystemTestSuite/src/Settings/player.h",
		L"./data/FileSystemTestSuite/player.h"));
	REQUIRE(isInFileInfos(
		files,
		L"./data/FileSystemTestSuite/src/Settings/sample.cpp",
		L"./data/FileSystemTestSuite/sample.cpp"));
	REQUIRE(isInFileInfos(
		files,
		L"./data/FileSystemTestSuite/src/main.cpp",
		L"./data/FileSystemTestSuite/src/Settings/src/main.cpp"));
	REQUIRE(isInFileInfos(
		files,
		L"./data/FileSystemTestSuite/src/test.cpp",
		L"./data/FileSystemTestSuite/src/Settings/src/test.cpp"));
	REQUIRE(isInFileInfos(
		files,
		L"./data/FileSystemTestSuite/src/test.h",
		L"./data/FileSystemTestSuite/src/Settings/src/test.h"));
#endif
}

TEST_CASE("find symlinked directories")
{
#ifndef _WIN32
	std::vector<FilePath> directoryPaths;
	directoryPaths.push_back(FilePath("./data/FileSystemTestSuite/src"));

	std::set<FilePath> dirs = FileSystem::getSymLinkedDirectories(directoryPaths);

	REQUIRE(dirs.size() == 2);
#endif
}
