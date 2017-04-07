#ifndef INCLUDES_DEFAULT_H
#define INCLUDES_DEFAULT_H

#include <QCoreApplication>
#include <QDir>
#include "utility/AppPath.h"
#include "utility/UserPaths.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "qt/utility/utilityQt.h"

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
	QDir coatiDir((userdir + "/.config/coati").c_str());
	userdir.append("/.config/sourcetrail/");

	UserPaths::setUserDataPath(userdir);

	QString userDataPath(userdir.c_str());
	QDir dataDir(userdir.c_str());
	if (!dataDir.exists())
	{
		if (coatiDir.exists())
		{
			utility::copyNewFilesFromDirectory(coatiDir.absolutePath(), userDataPath);
		}
		else
		{
			dataDir.mkpath(userDataPath);
		}
	}

	utility::copyNewFilesFromDirectory(QString::fromStdString(AppPath::getAppPath() + "/user/" ), userDataPath);
}

#endif // INCLUDES_DEFAULT_H
