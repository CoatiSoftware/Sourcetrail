#ifndef INCLUDES_DEFAULT_H
#define INCLUDES_DEFAULT_H

#include <QCoreApplication>
#include "utility/AppPath.h"
#include "utility/UserPaths.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "isTrial.h"

void setupPlatform(int argc, char *argv[])
{
}

void setupApp(int argc, char *argv[])
{
	if (AppPath::getAppPath().empty())
	{
		AppPath::setAppPath(QCoreApplication::applicationDirPath().toStdString() + "/");
	}

	std::string userdir(std::getenv("HOME"));
	if(isTrial())
	{
		userdir.append("/.config/coatitrial/");
	}
	else
	{
		userdir.append("/.config/coati/");
	}
    UserPaths::setUserDataPath(userdir);
	FilePath userDataPath(userdir);
	//copy default user folder to .config/coati if it does not exist
	if (!userDataPath.exists())
	{
		FilePath from(AppPath::getAppPath() + "user");
		FileSystem::copy_directory(from, userDataPath);
	}
}

#endif // INCLUDES_DEFAULT_H
