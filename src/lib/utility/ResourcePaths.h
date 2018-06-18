#ifndef RESOURCE_PATHS_H
#define RESOURCE_PATHS_H

#include <string>

#include "utility/file/FilePath.h"

class ResourcePaths
{
public:
	static FilePath getColorSchemesPath();
	static FilePath getFallbackPath();
	static FilePath getFontsPath();
	static FilePath getGuiPath();
	static FilePath getJavaPath();
	static FilePath getCxxCompilerHeaderPath();
};

#endif // RESOURCE_PATHS_H
