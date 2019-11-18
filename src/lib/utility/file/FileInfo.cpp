#include "FileInfo.h"

FileInfo::FileInfo(): path(FilePath(L"")) {}

FileInfo::FileInfo(const FilePath& path): path(path) {}

FileInfo::FileInfo(const FilePath& path, const TimeStamp& lastWriteTime)
	: path(path), lastWriteTime(lastWriteTime)
{
}
