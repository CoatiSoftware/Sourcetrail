#include "ResourcePaths.h"

#include "AppPath.h"

FilePath ResourcePaths::getColorSchemesPath()
{
	return FilePath(AppPath::getAppPath() + "data/color_schemes/");
}

FilePath ResourcePaths::getFallbackPath()
{
	return FilePath(AppPath::getAppPath() + "data/fallback/");
}

FilePath ResourcePaths::getFontsPath()
{
	return FilePath(AppPath::getAppPath() + "data/fonts/");
}

FilePath ResourcePaths::getGuiPath()
{
	return FilePath(AppPath::getAppPath() + "data/gui/");
}

FilePath ResourcePaths::getJavaPath()
{
	return FilePath(AppPath::getAppPath() + "data/java/");
}
