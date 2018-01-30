#include "utility/UserPaths.h"

FilePath UserPaths::s_userDataPath;

FilePath UserPaths::getUserDataPath()
{
	return s_userDataPath;
}

void UserPaths::setUserDataPath(const FilePath& path)
{
	s_userDataPath = path;
}

FilePath UserPaths::getAppSettingsPath()
{
	return getUserDataPath().concatenate(L"ApplicationSettings.xml");
}

FilePath UserPaths::getWindowSettingsPath()
{
	return getUserDataPath().concatenate(L"window_settings.ini");
}

FilePath UserPaths::getLogPath()
{
	return getUserDataPath().concatenate(L"log/");
}
