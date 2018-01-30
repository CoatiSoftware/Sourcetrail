#ifndef INCLUDES_DEFAULT_H
#define INCLUDES_DEFAULT_H

#include <QCoreApplication>
#include <QDir>

#include "qt/utility/utilityQt.h"
#include "utility/AppPath.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"

#include "settings/ApplicationSettings.h"

void setupPlatform(int argc, char *argv[])
{
	std::string home = std::getenv("HOME");
	UserPaths::setUserDataPath(FilePath(home + "/.config/sourcetrail/"));

	// Set QT screen scaling factor
	ApplicationSettings appSettings;
	appSettings.load(UserPaths::getAppSettingsPath());

	qputenv("QT_AUTO_SCREEN_SCALE_FACTOR_SOURCETRAIL", qgetenv("QT_AUTO_SCREEN_SCALE_FACTOR"));
	qputenv("QT_SCALE_FACTOR_SOURCETRAIL", qgetenv("QT_SCALE_FACTOR"));

	int autoScaling = appSettings.getScreenAutoScaling();
	if (autoScaling != -1)
	{
		QByteArray bytes;
		bytes.setNum(autoScaling);
		qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", bytes);
	}

	float scaleFactor = appSettings.getScreenScaleFactor();
	if (scaleFactor > 0.0)
	{
		QByteArray bytes;
		bytes.setNum(scaleFactor);
		qputenv("QT_SCALE_FACTOR", bytes);
	}
}

void setupApp(int argc, char *argv[])
{
	if (AppPath::getAppPath().empty())
	{
		AppPath::setAppPath(FilePath(QCoreApplication::applicationDirPath().toStdWString() + L"/"));
	}

	std::string userdir(std::getenv("HOME"));
	QDir coatiDir((userdir + "/.config/coati").c_str());
	userdir.append("/.config/sourcetrail/");

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

	utility::copyNewFilesFromDirectory(QString::fromStdWString(ResourcePaths::getFallbackPath().wstr()), userDataPath);
	utility::copyNewFilesFromDirectory(QString::fromStdWString(AppPath::getAppPath().concatenate(L"user/").wstr()), userDataPath);
}

#endif // INCLUDES_DEFAULT_H
