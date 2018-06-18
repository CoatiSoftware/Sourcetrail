#include "utility/utilityCxx.h"

#include "utility/ResourcePaths.h"
#include "utility/utilityApp.h"

namespace utility
{
	std::vector<FilePath> replaceOrAddCxxCompilerHeaderPath(const std::vector<FilePath>& headerSearchPaths)
	{
		if (utility::getOsType() == OS_WINDOWS)
		{
			return headerSearchPaths;
		}

		std::vector<FilePath> newHeaderSearchPaths;

		for (const FilePath& path : headerSearchPaths)
		{
			if (!path.getConcatenated(L"/stdarg.h").exists())
			{
				newHeaderSearchPaths.push_back(path);
			}
		}

		newHeaderSearchPaths.push_back(ResourcePaths::getCxxCompilerHeaderPath().getCanonical());
		return newHeaderSearchPaths;
	}
}
