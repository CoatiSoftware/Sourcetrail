#ifndef INCLUDES_MAC_H
#define INCLUDES_MAC_H

#include <CoreFoundation/CoreFoundation.h>
#include <QApplication>
#include <QDir>

void setup(int argc, char *argv[])
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
}

#endif // INCLUDES_MAC_H
