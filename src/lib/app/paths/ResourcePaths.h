#ifndef RESOURCE_PATHS_H
#define RESOURCE_PATHS_H

#include <string>

#include "FilePath.h"

class ResourcePaths
{
public:
	static FilePath getColorSchemesDirectoryPath();
	static FilePath getSyntaxHighlightingRulesDirectoryPath();
	static FilePath getFallbackDirectoryPath();
	static FilePath getFontsDirectoryPath();
	static FilePath getGuiDirectoryPath();
	static FilePath getLicenseDirectoryPath();
	static FilePath getJavaDirectoryPath();
	static FilePath getPythonDirectoryPath();
	static FilePath getCxxCompilerHeaderDirectoryPath();
	static FilePath getPythonIndexerFilePath();
};

#endif	  // RESOURCE_PATHS_H
