#include "utilityCxx.h"

#include "ResourcePaths.h"
#include "utilityApp.h"

namespace utility
{
	std::vector<FilePath> replaceOrAddCxxCompilerHeaderPath(const std::vector<FilePath>& headerSearchPaths)
	{
		std::vector<FilePath> newHeaderSearchPaths;

		if (utility::getOsType() == OS_WINDOWS)
		{
			newHeaderSearchPaths = headerSearchPaths;
		}
		else
		{
			for (const FilePath& path : headerSearchPaths)
			{
				if (!path.getConcatenated(L"/stdarg.h").exists())
				{
					newHeaderSearchPaths.push_back(path);
				}
			}
		}

		newHeaderSearchPaths.push_back(ResourcePaths::getCxxCompilerHeaderPath().getCanonical());
		return newHeaderSearchPaths;
	}
}
