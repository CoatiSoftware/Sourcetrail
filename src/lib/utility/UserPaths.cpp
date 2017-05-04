#include "utility/UserPaths.h"

FilePath UserPaths::s_userDataPath;
FilePath UserPaths::s_sampleProjectsPath;

FilePath UserPaths::getUserDataPath()
{
	return s_userDataPath;
}

void UserPaths::setUserDataPath(const FilePath& path)
{
	s_userDataPath = path;
}

FilePath UserPaths::getSampleProjectsPath()
{
	if (s_sampleProjectsPath.str().size())
	{
		return s_sampleProjectsPath;
	}

	return s_userDataPath;
}

void UserPaths::setSampleProjectsPath(const FilePath& path)
{
	s_sampleProjectsPath = path;
}

FilePath UserPaths::getAppSettingsPath()
{
	return getUserDataPath().concat(FilePath("ApplicationSettings.xml"));
}

FilePath UserPaths::getWindowSettingsPath()
{
	return getUserDataPath().concat(FilePath("window_settings.ini"));
}

FilePath UserPaths::getLogPath()
{
	return getUserDataPath().concat(FilePath("log/"));
}
