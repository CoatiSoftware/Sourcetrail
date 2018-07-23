#include "utility/path_detector/cxx_header/CxxHeaderPathDetector.h"

#include "utility/file/FilePath.h"
#include "utility/path_detector/cxx_header/utilityCxxHeaderDetection.h"
#include "utility/utilityString.h"
#include "utility/utilityCxx.h"

CxxHeaderPathDetector::CxxHeaderPathDetector(const std::string& compilerName)
	: PathDetector(compilerName)
	, m_compilerName(compilerName)
{
}

std::vector<FilePath> CxxHeaderPathDetector::getPaths() const
{
	std::vector<std::string> paths = utility::getCxxHeaderPaths(m_compilerName);
	std::vector<FilePath> headerSearchPaths;
	for (const std::string& path : paths)
	{
		if (!utility::isPostfix<std::string>(" (framework directory)", path))
		{
			headerSearchPaths.push_back(FilePath(path).makeCanonical());
		}
	}

	if (!headerSearchPaths.empty())
	{
		headerSearchPaths = utility::replaceOrAddCxxCompilerHeaderPath(headerSearchPaths);
	}

	return headerSearchPaths;
}
