#include "AppPath.h"

#include <string>

std::string AppPath::m_appPath = "";

std::string AppPath::getAppPath()
{
	if (m_appPath.length() <= 0)
	{
		setupAppPath();
	}

	return m_appPath;
}

void AppPath::setupAppPath()
{
#ifdef _WIN32
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);

	std::wstring wPath(path);
	m_appPath = std::string(wPath.begin(), wPath.end());

	size_t pos = m_appPath.find_last_of("/");
	if (pos == std::string::npos)
	{
		pos = m_appPath.find_last_of("\\");
	}
	if (pos != std::string::npos)
	{
		m_appPath = m_appPath.substr(0, pos + 1);
	}
#elif _WIN64
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);

	std::wstring wPath(path);
	m_appPath = std::string(wPath.begin(), wPath.end());

	size_t pos = m_appPath.find_last_of("/");
	if (pos == std::string::npos)
	{
		pos = m_appPath.find_last_of("\\");
	}
	if (pos != std::string::npos)
	{
		m_appPath = m_appPath.substr(0, pos + 1);
	}
#endif // WINDOWS
}

bool AppPath::setAppPath(std::string path)
{
	if(!path.empty())
	{
		m_appPath = path;
		return true;
	}
	return false;
}
