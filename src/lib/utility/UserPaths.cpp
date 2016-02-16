#include "utility/UserPaths.h"

std::string UserPaths::s_userDataPath = "user/";

std::string UserPaths::getUserDataPath()
{
	return s_userDataPath;
}

void UserPaths::setUserDataPath(const std::string& path)
{
	s_userDataPath = path;
}

std::string UserPaths::getAppSettingsPath()
{
	return getUserDataPath() + "ApplicationSettings.xml";
}

std::string UserPaths::getWindowSettingsPath()
{
	return getUserDataPath() + "window_settings.ini";
}

std::string UserPaths::getLogPath()
{
	return getUserDataPath() + "log/";
}
