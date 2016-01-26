#ifndef RESOURCE_PATHS_H
#define RESOURCE_PATHS_H

#include <string>

class ResourcePaths
{
public:
	static std::string getColorSchemesPath();
	static std::string getFontsPath();
	static std::string getGuiPath();

private:
	static std::string m_colorSchemesPath;
	static std::string m_fontsPath;
	static std::string m_guiPath;
};

#endif // RESOURCE_PATHS_H