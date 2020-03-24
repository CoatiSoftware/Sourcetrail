#ifndef APP_PATH_H
#define APP_PATH_H

#include "FilePath.h"

class AppPath
{
public:
	static FilePath getSharedDataPath();
	static bool setSharedDataPath(const FilePath& path);

	static FilePath getCxxIndexerPath();
	static bool setCxxIndexerPath(const FilePath& path);

private:
	static FilePath m_sharedDataPath;
	static FilePath m_cxxIndexerPath;
};

#endif	  // APP_PATH_H
