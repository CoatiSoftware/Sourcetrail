#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <string>
#include "boost/date_time.hpp"

struct FileInfo
{
	FileInfo(std::string path, boost::posix_time::ptime lastWriteTime);

	std::string path;
	boost::posix_time::ptime lastWriteTime;
};

#endif // FILE_INFO_H
