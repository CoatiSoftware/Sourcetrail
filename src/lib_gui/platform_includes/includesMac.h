#ifndef INCLUDES_MAC_H
#define INCLUDES_MAC_H

#include <CoreFoundation/CoreFoundation.h>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include "AppPath.h"
#include "FilePath.h"
#include "ResourcePaths.h"
#include "UserPaths.h"
#include "utilityQt.h"


void setupPlatform(int argc, char* argv[])
{
	UserPaths::setUserDataPath(FilePath(L"./user/").getAbsolute());

	// ----------------------------------------------------------------------------
	// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder
	// inside the .app bundle source:
	// http://stackoverflow.com/questions/516200/relative-paths-not-working-in-xcode-c
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
	if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)path, PATH_MAX))
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
		// printf("after change, libraryPaths=(%s)\n",
		// QCoreApplication::libraryPaths().join(",").toUtf8().data());
	}
	// ----------------------------------------------------------------------------


	// ----------------------------------------------------------------------------
	// Makes the mac bundle copy the user files to the Application Support folder
	QString dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	QString oldDataPath = QString::fromStdWString(ResourcePaths::getFallbackPath().wstr());

	QDir dataDir(dataPath);
	if (!dataDir.exists())
	{
		dataDir.mkpath(dataPath);
	}

	utility::copyNewFilesFromDirectory(oldDataPath, dataPath);

	// ----------------------------------------------------------------------------
	UserPaths::setUserDataPath(FilePath(dataPath.toStdWString() + L"/").getAbsolute());
}

void setupApp(int argc, char* argv[])
{
	const FilePath path(QDir::currentPath().toStdWString() + L"/");
	AppPath::setSharedDataPath(path.getAbsolute());
}

#endif	  // INCLUDES_MAC_H
