#ifndef INCLUDES_MAC_H
#define INCLUDES_MAC_H

#include <CoreFoundation/CoreFoundation.h>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include "qt/utility/utilityQt.h"
#include "utility/AppPath.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"

bool appIsMacBundle = false;

void setupPlatform(int argc, char *argv[])
{
	// ----------------------------------------------------------------------------
	// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
	// source: http://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	if (!mainBundle)
	{
		return;
	}

	CFStringRef bundleIdentifier = CFBundleGetIdentifier(mainBundle);
	// std::cout << CFStringGetCStringPtr(id, kCFStringEncodingASCII) << std::endl;
	if (!bundleIdentifier)
	{
		return;
	}

	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		chdir(path);
	}
	CFRelease(resourcesURL);
	// ----------------------------------------------------------------------------


	// ----------------------------------------------------------------------------
	// This makes the mac bundle search in the right place for the cocoa plugin
	// source: http://qt-project.org/doc/qt-4.8/deployment-mac.html
	QDir dir(argv[0]);
	if (dir.cd("../../PlugIns"))
	{
		QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
		// printf("after change, libraryPaths=(%s)\n", QCoreApplication::libraryPaths().join(",").toUtf8().data());
	}
	// ----------------------------------------------------------------------------


	// ----------------------------------------------------------------------------
	// Makes the mac bundle copy the user files to the Application Support folder
	QString dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString oldDataPath = QString::fromStdString(ResourcePaths::getFallbackPath());

	QDir dataDir(dataPath);
	if (!dataDir.exists())
	{
		dataDir.mkpath(dataPath);

		if (dataPath.endsWith("Sourcetrail"))
		{
			QString coatiDataPath = dataPath;
			coatiDataPath.append("/../Coati");

			QDir coatiDataDir(coatiDataPath);
			if (coatiDataDir.exists())
			{
				oldDataPath = coatiDataPath;
			}
		}
	}

	utility::copyNewFilesFromDirectory(oldDataPath, dataPath);

	UserPaths::setUserDataPath(dataPath.toStdString() + "/");
	// ----------------------------------------------------------------------------

	appIsMacBundle = true;
}

void setupApp(int argc, char *argv[])
{
	FilePath path(QDir::currentPath().toStdString());
	AppPath::setAppPath(path.absolute().str() + "/");

	if (!appIsMacBundle)
	{
		UserPaths::setUserDataPath(path.absolute().str() + "/user/");
	}
	else
	{
		UserPaths::setSampleProjectsPath(path.absolute().str() + "/data/");
	}
}

#endif // INCLUDES_MAC_H
