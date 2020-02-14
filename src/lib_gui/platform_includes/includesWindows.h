#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <QDir>
#include <string>
#include <windows.h>

#include "AppPath.h"
#include "Application.h"
#include "FileSystem.h"
#include "ResourcePaths.h"
#include "UserPaths.h"
#include "logging.h"
#include "utility.h"
#include "utilityApp.h"

void setupPlatform(int argc, char* argv[]) {}

void setupApp(int argc, char* argv[])
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
		AppPath::setAppPath(FilePath(appPath));
	}

	{
		FilePath userDataPath = AppPath::getAppPath().concatenate(L"user/");
		if (!userDataPath.exists())
		{
#pragma warning(push)
#pragma warning(disable : 4996)
			FilePath userLocalPath = FilePath(std::string(std::getenv("LOCALAPPDATA")));
			if (!userLocalPath.exists())
			{
				userLocalPath = FilePath(std::string(std::getenv("APPDATA")) + "/../local");
			}
#pragma warning(pop)

			if (userLocalPath.exists())
			{
				userDataPath = userLocalPath.getConcatenated(L"Coati Software/");
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
			else
			{
				userDataPath = AppPath::getAppPath().concatenate(L"user_fallback/");
				LOG_ERROR(
					L"The \"%LOCALAPPDATA%\" path could not be found. Falling back to \"" +
					userDataPath.wstr() + L"\" to store settings data.");
				FileSystem::createDirectory(userDataPath);
			}
		}

		UserPaths::setUserDataPath(userDataPath);
	}

	// This "copyFile" method does nothing if the copy destination already exist
	FileSystem::copyFile(
		ResourcePaths::getFallbackPath().concatenate(L"ApplicationSettings.xml"),
		UserPaths::getAppSettingsPath());
	FileSystem::copyFile(
		ResourcePaths::getFallbackPath().concatenate(L"window_settings.ini"),
		UserPaths::getWindowSettingsPath());
}

#endif	  // INCLUDES_WINDOWS_H
