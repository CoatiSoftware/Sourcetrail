#ifndef APP_PATH_H
#define APP_PATH_H

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif // _WIN32

class AppPath
{
public:
	static std::string getAppPath();
	static bool setAppPath(std::string path);

private:
	static std::string m_appPath;
};

#endif // APP_PATH_H
