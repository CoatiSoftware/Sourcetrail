#ifndef INCLUDES_DEFAULT_H
#define INCLUDES_DEFAULT_H

#include <QCoreApplication>
#include <QDir>
#include "utility/AppPath.h"
#include "utility/UserPaths.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "qt/utility/utilityQt.h"
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

	QString userDataPath(userdir.c_str());
	QDir dataDir(userdir.c_str());
	if (!dataDir.exists())
	{
		dataDir.mkpath(userDataPath);
	}

	utility::copyNewFilesFromDirectory(QString::fromStdString(UserPaths::getUserDataPath()), userDataPath);
}

#endif // INCLUDES_DEFAULT_H
