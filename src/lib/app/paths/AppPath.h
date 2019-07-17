#ifndef APP_PATH_H
#define APP_PATH_H

#include "FilePath.h"

class AppPath
{
public:
	static FilePath getAppPath();
	static bool setAppPath(const FilePath& path);

private:
	static FilePath m_appPath;
};

#endif // APP_PATH_H
