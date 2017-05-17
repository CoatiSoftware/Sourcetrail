#include "utility/path_detector/cxx_header/CxxFrameworkPathDetector.h"

#include "utility/file/FilePath.h"
#include "utility/path_detector/cxx_header/utilityCxxHeaderDetection.h"
#include "utility/utilityString.h"

CxxFrameworkPathDetector::CxxFrameworkPathDetector(const std::string& compilerName)
	: PathDetector(compilerName)
	, m_compilerName(compilerName)
{
}

CxxFrameworkPathDetector::~CxxFrameworkPathDetector()
{
}

std::vector<FilePath> CxxFrameworkPathDetector::getPaths() const
{
	std::vector<std::string> paths = utility::getCxxHeaderPaths(m_compilerName);
	std::vector<FilePath> frameworkPaths;
	for (const std::string& path : paths)
	{
		if (utility::isPostfix(" (framework directory)", path))
		{
			frameworkPaths.push_back(FilePath(utility::replace(path, " (framework directory)", "")).canonical());
		}
	}
	return frameworkPaths;
}
