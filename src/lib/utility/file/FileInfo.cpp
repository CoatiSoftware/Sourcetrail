#include "FileInfo.h"

FileInfo::FileInfo()
	: path(FilePath(""))
{
}

FileInfo::FileInfo(const FilePath& path)
	: path(path)
{
}

FileInfo::FileInfo(const FilePath& path, const TimePoint& lastWriteTime)
	: path(path)
	, lastWriteTime(lastWriteTime)
{
}
