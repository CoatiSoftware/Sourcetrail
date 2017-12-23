#include "utility/path_detector/cxx_header/CxxHeaderPathDetector.h"

#include "utility/file/FilePath.h"
#include "utility/path_detector/cxx_header/utilityCxxHeaderDetection.h"
#include "utility/utilityString.h"

CxxHeaderPathDetector::CxxHeaderPathDetector(const std::string& compilerName)
	: PathDetector(compilerName)
	, m_compilerName(compilerName)
{
}

CxxHeaderPathDetector::~CxxHeaderPathDetector()
{
}

std::vector<FilePath> CxxHeaderPathDetector::getPaths() const
{
	std::vector<std::string> paths = utility::getCxxHeaderPaths(m_compilerName);
	std::vector<FilePath> headerPaths;
	for (const std::string& path : paths)
	{
		if (!utility::isPostfix(" (framework directory)", path))
		{
			headerPaths.push_back(FilePath(path).makeCanonical());
		}
	}
	return headerPaths;
}
