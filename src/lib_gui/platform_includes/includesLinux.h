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
	userdir.append("/.config/coati/");

	UserPaths::setUserDataPath(userdir);

	QString userDataPath(userdir.c_str());
	QDir dataDir(userdir.c_str());
	if (!dataDir.exists())
	{
		dataDir.mkpath(userDataPath);
	}

	std::cout << "apppath: " << AppPath::getAppPath() << std::endl;

	utility::copyNewFilesFromDirectory(QString::fromStdString(AppPath::getAppPath() + "/usr/" ), userDataPath);
}

#endif // INCLUDES_DEFAULT_H
