#include "ResourcePaths.h"

#include "AppPath.h"

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

FilePath ResourcePaths::getPythonPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/python/");
}

FilePath ResourcePaths::getCxxCompilerHeaderPath()
{
	return AppPath::getSharedDataPath().concatenate(L"data/cxx/include/").getCanonical();
}
