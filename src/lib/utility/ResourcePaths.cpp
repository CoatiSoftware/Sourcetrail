#include "ResourcePaths.h"

#include "AppPath.h"

std::string ResourcePaths::m_colorSchemesPath = AppPath::getAppPath() + "data/color_schemes/";
std::string ResourcePaths::m_fontsPath = AppPath::getAppPath() + "data/fonts/";
std::string ResourcePaths::m_guiPath = AppPath::getAppPath() + "data/gui/";

std::string ResourcePaths::getColorSchemesPath()
{
	return m_colorSchemesPath;
}

std::string ResourcePaths::getFontsPath()
{
	return m_fontsPath;
}

std::string ResourcePaths::getGuiPath()
{
	return m_guiPath;
}