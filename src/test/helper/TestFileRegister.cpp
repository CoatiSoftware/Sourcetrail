#include "TestFileRegister.h"

#include "../../lib/utility/file/FilePathFilter.h"

TestFileRegister::TestFileRegister()
	: FileRegister(FilePath(), std::set<FilePath>(), {FilePathFilter(L"")})
{
}

TestFileRegister::~TestFileRegister() {}

bool TestFileRegister::hasFilePath(const FilePath& filePath) const
{
	return true;
}
