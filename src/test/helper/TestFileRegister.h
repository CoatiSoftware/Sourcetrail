#ifndef TEST_FILE_MANAGER_H
#define TEST_FILE_MANAGER_H

#include "utility/file/FileRegister.h"

class TestFileRegister
	: public FileRegister
{
public:
	TestFileRegister();
	virtual ~TestFileRegister();

	virtual bool fileIsIndexed(const FilePath& filePath) const;
	virtual bool hasFilePath(const FilePath& filePath) const;
};

#endif // TEST_FILE_MANAGER_H
