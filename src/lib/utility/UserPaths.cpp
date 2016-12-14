#include "utility/UserPaths.h"

std::string UserPaths::s_userDataPath = "";
std::string UserPaths::s_sampleProjectsPath = "";

std::string UserPaths::getUserDataPath()
{
	return s_userDataPath;
}

void UserPaths::setUserDataPath(const std::string& path)
{
	s_userDataPath = path;
}

std::string UserPaths::getSampleProjectsPath()
{
	if (s_sampleProjectsPath.size())
	{
		return s_sampleProjectsPath;
	}

	return s_userDataPath;
}

void UserPaths::setSampleProjectsPath(const std::string& path)
{
	s_sampleProjectsPath = path;
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
