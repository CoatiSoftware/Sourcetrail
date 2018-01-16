#include "AppPath.h"

#include <string>

std::string AppPath::m_appPath = "";

std::string AppPath::getAppPath()
{
	return m_appPath;
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
