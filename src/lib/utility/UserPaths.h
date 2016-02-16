#ifndef USER_PATHS_H
#define USER_PATHS_H

#include <string>

class UserPaths
{
public:
	static std::string getUserDataPath();
	static void setUserDataPath(const std::string& path);

	static std::string getAppSettingsPath();
	static std::string getWindowSettingsPath();
	static std::string getLogPath();

private:
	static std::string s_userDataPath;
};

#endif // USER_PATHS_H
