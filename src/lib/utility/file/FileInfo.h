#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <string>

#include "boost/date_time.hpp"

#include "utility/file/FilePath.h"

struct FileInfo
{
	FileInfo(const FilePath& path, boost::posix_time::ptime lastWriteTime);

	FilePath path;
	boost::posix_time::ptime lastWriteTime;
};

#endif // FILE_INFO_H
