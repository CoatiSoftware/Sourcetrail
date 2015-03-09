#include "TestFileManager.h"

TestFileManager::TestFileManager()
	: FileManager(
		std::vector<std::string>(),
		std::vector<std::string>(),
		std::vector<std::string>(),
		std::vector<std::string>()
	)
{
}

bool TestFileManager::hasFilePath(const std::string& filePath) const
{
	return true;
}

bool TestFileManager::hasSourceExtension(const std::string& filePath) const
{
	return true;
}

bool TestFileManager::hasIncludeExtension(const std::string& filePath) const
{
	return true;
}
