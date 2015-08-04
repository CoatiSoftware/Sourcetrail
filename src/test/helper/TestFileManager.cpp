#include "TestFileManager.h"

TestFileManager::TestFileManager()
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
