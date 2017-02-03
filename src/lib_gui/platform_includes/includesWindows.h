#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <string>
#include <QDir>

#include "vld.h"

#include "utility/file/FileSystem.h"
#include "utility/UserPaths.h"
#include "utility/utilityApp.h"

#include "platform_includes/deploy.h"

void setupPlatform(int argc, char *argv[])
{
}

void setupApp(int argc, char *argv[])
{
#ifdef DEPLOY
	std::string path = std::getenv("APPDATA");
	path += "/../local/Coati Software/";
	if (utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_64)
	{
		path += "Coati 64-bit/";
	}
	else
	{
		path += "Coati/";
	}
	UserPaths::setUserDataPath(path);
#else
	std::string path = QDir::currentPath().toStdString();
	path += "/user/";
	UserPaths::setUserDataPath(path);
#endif

	// These method does nothing if the copy destination already exist
	FileSystem::copyFile("data/fallback/ApplicationSettings.xml", UserPaths::getAppSettingsPath());
	FileSystem::copyFile("data/fallback/window_settings.ini", UserPaths::getWindowSettingsPath());
}

#endif // INCLUDES_WINDOWS_H
