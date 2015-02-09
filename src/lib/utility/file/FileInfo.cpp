#include "FileInfo.h"

FileInfo::FileInfo(std::string path, boost::posix_time::ptime lastWriteTime)
	: path(path)
	, lastWriteTime(lastWriteTime)
{
}
