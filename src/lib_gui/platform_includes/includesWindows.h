#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <string>
#include <QDir>
#include <windows.h>

#include "vld.h"

#include "utility/file/FileSystem.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"
#include "utility/utility.h"

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
		path += "Sourcetrail 64-bit/";
	}
	else
	{
		path += "Sourcetrail/";
	}
	UserPaths::setUserDataPath(FilePath(path));
#else
	std::string path = QDir::currentPath().toStdString();
	path += "/user/";
	UserPaths::setUserDataPath(FilePath(path));
#endif

	// This "copyFile" method does nothing if the copy destination already exist
	FileSystem::copyFile(ResourcePaths::getFallbackPath().concatenate(FilePath("ApplicationSettings.xml")), UserPaths::getAppSettingsPath());
	FileSystem::copyFile(ResourcePaths::getFallbackPath().concatenate(FilePath("window_settings.ini")), UserPaths::getWindowSettingsPath());
}

#endif // INCLUDES_WINDOWS_H
