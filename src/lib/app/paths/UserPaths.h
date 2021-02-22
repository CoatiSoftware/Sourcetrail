#ifndef USER_PATHS_H
#define USER_PATHS_H

#include <string>

#include "FilePath.h"

class UserPaths
{
public:
	static FilePath getUserDataDirectoryPath();
	static void setUserDataDirectoryPath(const FilePath& path);

	static FilePath getAppSettingsFilePath();
	static FilePath getWindowSettingsFilePath();
	static FilePath getLogDirectoryPath();

private:
	static FilePath s_userDataDirectoryPath;
};

#endif	  // USER_PATHS_H
