#ifndef RESOURCE_PATHS_H
#define RESOURCE_PATHS_H

#include <string>

class ResourcePaths
{
public:
	static std::string getColorSchemesPath();
	static std::string getFallbackPath();
	static std::string getFontsPath();
	static std::string getGuiPath();
	static std::string getJavaPath();
};

#endif // RESOURCE_PATHS_H
