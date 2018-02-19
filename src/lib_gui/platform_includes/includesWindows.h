#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <string>
#include <QDir>
#include <windows.h>

#include "vld.h"

#include "utility/file/FileSystem.h"
#include "utility/AppPath.h"
#include "utility/ResourcePaths.h"
#include "utility/UserPaths.h"
#include "utility/utility.h"

void setupPlatform(int argc, char *argv[])
{
}

void setupApp(int argc, char *argv[])
{
	{
		HMODULE hModule = GetModuleHandleW(NULL);
		WCHAR path[MAX_PATH];
		GetModuleFileNameW(hModule, path, MAX_PATH);

		std::wstring appPath(path);

		size_t pos = appPath.find_last_of(L"/");
		if (pos == std::wstring::npos)
		{
			pos = appPath.find_last_of(L"\\");
		}
		if (pos != std::wstring::npos)
		{
			appPath = appPath.substr(0, pos + 1);
		}
		AppPath::setAppPath(FilePath(L"C:/sourcetrail/täst/Sourcetrail_2018_1_19_32bit")); 
		//AppPath::setAppPath(FilePath(appPath)); C:\sourcetrail\täst\Sourcetrail_2018_1_19_32bit
	}

	{
		FilePath userDataPath = AppPath::getAppPath().concatenate(L"user/");
		if (!userDataPath.exists())
		{
			userDataPath = FilePath(std::string(std::getenv("APPDATA")) + "/../local/Coati Software/");
			if (utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_64)
			{
				userDataPath.concatenate(L"Sourcetrail 64-bit/");
			}
			else
			{
				userDataPath.concatenate(L"Sourcetrail/");
			}
			userDataPath.makeCanonical();
		}
		UserPaths::setUserDataPath(userDataPath);
	}

	// This "copyFile" method does nothing if the copy destination already exist
	FileSystem::copyFile(ResourcePaths::getFallbackPath().concatenate(L"ApplicationSettings.xml"), UserPaths::getAppSettingsPath());
	FileSystem::copyFile(ResourcePaths::getFallbackPath().concatenate(L"window_settings.ini"), UserPaths::getWindowSettingsPath());
}

#endif // INCLUDES_WINDOWS_H
