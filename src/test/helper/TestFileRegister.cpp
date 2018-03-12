#include "TestFileRegister.h"

#include "utility/file/FilePathFilter.h"

TestFileRegister::TestFileRegister()
	: FileRegister(FileRegisterStateData(), FilePath(), std::set<FilePath>(), { FilePathFilter(L"") })
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
