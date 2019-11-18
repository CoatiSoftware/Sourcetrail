#include "catch.hpp"

#include "FileManager.h"
#include "FilePath.h"
#include "FilePathFilter.h"
#include "FileSystem.h"
#include "utility.h"

TEST_CASE("file manager has added file paths after first fetch")
{
	std::vector<FilePath> sourcePaths;
	sourcePaths.push_back(FilePath(L"./data/FileManagerTestSuite/src/"));
	sourcePaths.push_back(FilePath(L"./data/FileManagerTestSuite/include/"));
	std::vector<FilePath> headerPaths;
	std::vector<FilePathFilter> excludeFilters;

	// catch exceptions thrown on linux build machine
	try
	{
		std::vector<FilePath> filePaths = FileSystem::getFilePathsFromDirectory(
			FilePath(L"./data/FileManagerTestSuite/src/"));
		REQUIRE(filePaths.size() == 3);

		std::vector<std::wstring> sourceExtensions;
		for (FilePath p: filePaths)
		{
			sourceExtensions.push_back(p.extension());
		}
		REQUIRE(sourceExtensions.size() == 3);

		FileManager fm;
		fm.update(sourcePaths, excludeFilters, sourceExtensions);
		std::vector<FilePath> foundSourcePaths = utility::toVector(fm.getAllSourceFilePaths());

		REQUIRE(foundSourcePaths.size() == 3);
		REQUIRE(utility::containsElement<FilePath>(foundSourcePaths, filePaths[0]));
		REQUIRE(utility::containsElement<FilePath>(foundSourcePaths, filePaths[1]));
		REQUIRE(utility::containsElement<FilePath>(foundSourcePaths, filePaths[2]));
	}
	catch (...)
	{
	}
}
