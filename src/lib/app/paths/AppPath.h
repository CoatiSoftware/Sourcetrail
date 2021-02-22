#ifndef APP_PATH_H
#define APP_PATH_H

#include "FilePath.h"

class AppPath
{
public:
	static FilePath getSharedDataDirectoryPath();
	static bool setSharedDataDirectoryPath(const FilePath& path);

	static FilePath getCxxIndexerFilePath();
	static bool setCxxIndexerDirectoryPath(const FilePath& path);

private:
	static FilePath s_sharedDataDirectoryPath;
	static FilePath s_cxxIndexerDirectoryPath;
};

#endif	  // APP_PATH_H
