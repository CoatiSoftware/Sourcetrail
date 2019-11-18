#ifndef RESOURCE_PATHS_H
#define RESOURCE_PATHS_H

#include <string>

#include "FilePath.h"

class ResourcePaths
{
public:
	static FilePath getColorSchemesPath();
	static FilePath getSyntaxHighlightingRulesPath();
	static FilePath getFallbackPath();
	static FilePath getFontsPath();
	static FilePath getGuiPath();
	static FilePath getLicensePath();
	static FilePath getJavaPath();
	static FilePath getPythonPath();
	static FilePath getCxxCompilerHeaderPath();
};

#endif	  // RESOURCE_PATHS_H
