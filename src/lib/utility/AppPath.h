#ifndef APP_PATH_H
#define APP_PATH_H

#include <string>

// #include "src\lib_gui\includes.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#elif _WIN64
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif // WINDOWS


class AppPath
{
public:
	static std::string getAppPath();
	static bool setAppPath(std::string path);

private:
	static void setupAppPath();

	static std::string m_appPath;
};

#endif // APP_PATH_H