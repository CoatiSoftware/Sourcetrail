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
	static FilePath getPythonEnvironmentFilePath();
	static FilePath getPythonIndexerFilePath();
	static FilePath getCxxCompilerHeaderPath();

private:
	static FilePath getPythonPath();
};

#endif	  // RESOURCE_PATHS_H
