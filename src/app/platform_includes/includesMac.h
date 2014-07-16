#ifndef INCLUDES_MAC_H
#define INCLUDES_MAC_H

#include <CoreFoundation/CoreFoundation.h>

void setup()
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
}

#endif // INCLUDES_MAC_H
