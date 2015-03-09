#include "FileInfo.h"

FileInfo::FileInfo(const FilePath& path, boost::posix_time::ptime lastWriteTime)
	: path(path)
	, lastWriteTime(lastWriteTime)
{
}
