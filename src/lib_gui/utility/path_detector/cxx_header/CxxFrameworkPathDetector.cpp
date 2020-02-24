#include "CxxFrameworkPathDetector.h"

#include "FilePath.h"
#include "utilityCxxHeaderDetection.h"
#include "utilityString.h"

CxxFrameworkPathDetector::CxxFrameworkPathDetector(const std::string& compilerName)
	: PathDetector(compilerName), m_compilerName(compilerName)
{
}

std::vector<FilePath> CxxFrameworkPathDetector::doGetPaths() const
{
	std::vector<std::string> paths = utility::getCxxHeaderPaths(m_compilerName);
	std::vector<FilePath> frameworkPaths;
	for (const std::string& path: paths)
	{
		if (utility::isPostfix<std::string>(" (framework directory)", path))
		{
			FilePath p =
				FilePath(utility::replace(path, " (framework directory)", "")).makeCanonical();
			if (p.exists())
			{
				frameworkPaths.push_back(p);
			}
		}
	}
	return frameworkPaths;
}
