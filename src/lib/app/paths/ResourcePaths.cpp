#include "ResourcePaths.h"

#include "AppPath.h"
#include "utilityApp.h"

FilePath ResourcePaths::getColorSchemesPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/color_schemes/");
}

FilePath ResourcePaths::getSyntaxHighlightingRulesPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/syntax_highlighting_rules/");
}

FilePath ResourcePaths::getFallbackPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/fallback/");
}

FilePath ResourcePaths::getFontsPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/fonts/");
}

FilePath ResourcePaths::getGuiPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/gui/");
}

FilePath ResourcePaths::getLicensePath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/license/");
}

FilePath ResourcePaths::getJavaPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/java/");
}

FilePath ResourcePaths::getPythonDirectoryPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/python/");
}

FilePath ResourcePaths::getCxxCompilerHeaderPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/cxx/include/").getCanonical();
}

FilePath ResourcePaths::getPythonIndexerFilePath()
{
	if (utility::getOsType() == OS_WINDOWS)
	{
		return getPythonDirectoryPath().concatenate(L"SourcetrailPythonIndexer.exe");
	}
	return getPythonDirectoryPath().concatenate(L"SourcetrailPythonIndexer");
}
