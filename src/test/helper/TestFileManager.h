#ifndef TEST_FILE_MANAGER_H
#define TEST_FILE_MANAGER_H

#include "utility/file/FileManager.h"

class TestFileManager: public FileManager
{
public:
	TestFileManager();

	virtual bool hasFilePath(const FilePath& filePath) const;
	virtual bool hasSourceExtension(const FilePath& filePath) const;
	virtual bool hasIncludeExtension(const FilePath& filePath) const;
};

#endif // TEST_FILE_MANAGER_H
