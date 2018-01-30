#include "AppPath.h"

FilePath AppPath::m_appPath(L"");

FilePath AppPath::getAppPath()
{
	return m_appPath;
}

bool AppPath::setAppPath(const FilePath& path)
{
	if(!path.empty())
	{
		m_appPath = path;
		return true;
	}
	return false;
}
