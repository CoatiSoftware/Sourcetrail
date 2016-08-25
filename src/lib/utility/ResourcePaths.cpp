#include "ResourcePaths.h"

#include "AppPath.h"

std::string ResourcePaths::getColorSchemesPath()
{
	return AppPath::getAppPath() + "data/color_schemes/";
}

std::string ResourcePaths::getFontsPath()
{
	return AppPath::getAppPath() + "data/fonts/";
}

std::string ResourcePaths::getGuiPath()
{
	return AppPath::getAppPath() + "data/gui/";
}

std::string ResourcePaths::getJavaPath()
{
	return AppPath::getAppPath() + "data/java/";
}
