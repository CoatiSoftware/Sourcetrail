#include "TestFileManager.h"

TestFileManager::TestFileManager()
{
}

bool TestFileManager::hasFilePath(const FilePath& filePath) const
{
	return true;
}

bool TestFileManager::hasSourceFilePath(const FilePath& filePath) const
{
	return true;
}

const FileInfo TestFileManager::getFileInfo(const FilePath& filePath) const
{
	return FileInfo(filePath);
}
