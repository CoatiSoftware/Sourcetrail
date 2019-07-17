#include "ResourcePaths.h"

#include "AppPath.h"

FilePath ResourcePaths::getColorSchemesPath()
{
	return AppPath::getAppPath().concatenate(L"data/color_schemes/");
}

FilePath ResourcePaths::getSyntaxHighlightingRulesPath()
{
	return AppPath::getAppPath().concatenate(L"data/syntax_highlighting_rules/");
}

FilePath ResourcePaths::getFallbackPath()
{
	return AppPath::getAppPath().concatenate(L"data/fallback/");
}

FilePath ResourcePaths::getFontsPath()
{
	return AppPath::getAppPath().concatenate(L"data/fonts/");
}

FilePath ResourcePaths::getGuiPath()
{
	return AppPath::getAppPath().concatenate(L"data/gui/");
}

FilePath ResourcePaths::getLicensePath()
{
	return AppPath::getAppPath().concatenate(L"data/license/");
}

FilePath ResourcePaths::getJavaPath()
{
	return AppPath::getAppPath().concatenate(L"data/java/");
}

FilePath ResourcePaths::getPythonPath()
{
	return AppPath::getAppPath().concatenate(L"data/python/");
}

FilePath ResourcePaths::getCxxCompilerHeaderPath()
{
	return AppPath::getAppPath().concatenate(L"data/cxx/include/").getCanonical();
}
