#ifndef USER_PATHS_H
#define USER_PATHS_H

#include <string>

#include "utility/file/FilePath.h"

class UserPaths
{
public:
	static FilePath getUserDataPath();
	static void setUserDataPath(const FilePath& path);

	static FilePath getAppSettingsPath();
	static FilePath getWindowSettingsPath();
	static FilePath getLogPath();

private:
	static FilePath s_userDataPath;
};

#endif // USER_PATHS_H
