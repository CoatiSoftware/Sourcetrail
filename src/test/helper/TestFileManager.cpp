#include "TestFileManager.h"

TestFileManager::TestFileManager()
	: FileManager(
		std::vector<FilePath>(),
		std::vector<FilePath>(),
		std::vector<std::string>(),
		std::vector<std::string>()
	)
{
}

bool TestFileManager::hasFilePath(const FilePath& filePath) const
{
	return true;
}

bool TestFileManager::hasSourceExtension(const FilePath& filePath) const
{
	return true;
}

bool TestFileManager::hasIncludeExtension(const FilePath& filePath) const
{
	return true;
}
