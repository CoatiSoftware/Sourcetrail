#include "FileInfo.h"

FileInfo::FileInfo()
	: path(FilePath(""))
	, lastWriteTime(boost::posix_time::not_a_date_time)
{
}

FileInfo::FileInfo(const FilePath& path)
	: path(path)
	, lastWriteTime(boost::posix_time::not_a_date_time)
{
}

FileInfo::FileInfo(const FilePath& path, boost::posix_time::ptime lastWriteTime)
	: path(path)
	, lastWriteTime(lastWriteTime)
{
}
