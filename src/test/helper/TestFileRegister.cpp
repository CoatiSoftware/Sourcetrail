#include "TestFileRegister.h"

TestFileRegister::TestFileRegister()
	: FileRegister(FileRegisterStateData(), std::set<FilePath>(), std::set<FilePath>())
{
}

TestFileRegister::~TestFileRegister()
{
}

bool TestFileRegister::fileIsIndexed(const FilePath& filePath) const
{
	return false;
}

bool TestFileRegister::hasFilePath(const FilePath& filePath) const
{
	return true;
}
