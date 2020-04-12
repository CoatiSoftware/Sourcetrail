#include "AppPath.h"

FilePath AppPath::m_sharedDataPath(L"");
FilePath AppPath::m_cxxIndexerPath(L"");

FilePath AppPath::getSharedDataPath()
{
	return m_sharedDataPath;
}

bool AppPath::setSharedDataPath(const FilePath& path)
{
	if (!path.empty())
	{
		m_sharedDataPath = path;
		return true;
	}
	return false;
}

FilePath AppPath::getCxxIndexerPath()
{
#if _WIN32
	const std::wstring cxxIndexerName(L"sourcetrail_indexer.exe");
#else
	const std::wstring cxxIndexerName(L"sourcetrail_indexer");
#endif

	if (!m_cxxIndexerPath.empty())
	{
		return m_cxxIndexerPath.getConcatenated(cxxIndexerName);
	}
	return m_sharedDataPath.getConcatenated(cxxIndexerName);
}

bool AppPath::setCxxIndexerPath(const FilePath& path)
{
	if (!path.empty())
	{
		m_cxxIndexerPath = path;
		return true;
	}
	return false;
}
