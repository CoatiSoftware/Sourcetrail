#ifndef INCLUDES_DEFAULT_H
#define INCLUDES_DEFAULT_H

#include <QCoreApplication>
#include <QDir>

#include "AppPath.h"
#include "FilePath.h"
#include "FileSystem.h"
#include "ResourcePaths.h"
#include "UserPaths.h"
#include "utilityQt.h"

#include "ApplicationSettings.h"

void setupPlatform(int argc, char* argv[])
{
	std::string home = std::getenv("HOME");
	UserPaths::setUserDataPath(FilePath(home + "/.config/sourcetrail/"));

	// Set QT screen scaling factor
	ApplicationSettings appSettings;
	appSettings.load(UserPaths::getAppSettingsPath(), true);

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

void setupApp(int argc, char* argv[])
{
	FilePath appPath = FilePath(QCoreApplication::applicationDirPath().toStdWString() + L"/").getAbsolute();
	AppPath::setSharedDataPath(appPath);
	AppPath::setCxxIndexerPath(appPath);

	// Check if bundled as Linux AppImage
	if (appPath.getConcatenated(L"/../share/data").exists())
	{
		AppPath::setSharedDataPath(appPath.getConcatenated(L"/../share").getAbsolute());
	}

	std::string userdir(std::getenv("HOME"));
	userdir.append("/.config/sourcetrail/");

	QString userDataPath(userdir.c_str());
	QDir dataDir(userdir.c_str());
	if (!dataDir.exists())
	{
		dataDir.mkpath(userDataPath);
	}

	utility::copyNewFilesFromDirectory(
		QString::fromStdWString(ResourcePaths::getFallbackPath().wstr()), userDataPath);
	utility::copyNewFilesFromDirectory(
		QString::fromStdWString(AppPath::getSharedDataPath().concatenate(L"user/").wstr()), userDataPath);
}

#endif	  // INCLUDES_DEFAULT_H
