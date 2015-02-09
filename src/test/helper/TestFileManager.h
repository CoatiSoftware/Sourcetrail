#ifndef TEST_FILE_MANAGER_H
#define TEST_FILE_MANAGER_H

#include "utility/file/FileManager.h"

class TestFileManager: public FileManager
{
public:
	TestFileManager();
	virtual bool hasFilePath(const std::string& filePath) const;
};

#endif // TEST_FILE_MANAGER_H
