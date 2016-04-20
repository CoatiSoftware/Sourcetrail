#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <string>

#include "boost/date_time.hpp"

#include "utility/file/FilePath.h"
#include "utility/TimePoint.h"

struct FileInfo
{
	FileInfo();
	FileInfo(const FilePath& path);
	FileInfo(const FilePath& path, const TimePoint& lastWriteTime);

	FilePath path;
	TimePoint lastWriteTime;
};

#endif // FILE_INFO_H
