#include "utilityCxx.h"

#include "ResourcePaths.h"

namespace utility
{
	std::vector<FilePath> replaceOrAddCxxCompilerHeaderPath(const std::vector<FilePath>& headerSearchPaths)
	{
		std::vector<FilePath> newHeaderSearchPaths;
		const FilePath cxxCompilerHeaderPath = ResourcePaths::getCxxCompilerHeaderPath();

		for (const FilePath& path : headerSearchPaths)
		{
			if (path != cxxCompilerHeaderPath)
			{
				newHeaderSearchPaths.push_back(path);
			}
		}

		newHeaderSearchPaths.push_back(cxxCompilerHeaderPath);
		return newHeaderSearchPaths;
	}
}
