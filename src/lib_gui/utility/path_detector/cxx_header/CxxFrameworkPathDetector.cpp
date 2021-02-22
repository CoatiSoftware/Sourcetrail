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
	std::vector<std::wstring> paths = utility::getCxxHeaderPaths(m_compilerName);
	std::vector<FilePath> frameworkPaths;
	for (const std::wstring& path: paths)
	{
		if (utility::isPostfix<std::wstring>(L" (framework directory)", path))
		{
			FilePath p =
				FilePath(utility::replace(path, L" (framework directory)", L"")).makeCanonical();
			if (p.exists())
			{
				frameworkPaths.push_back(p);
			}
		}
	}
	return frameworkPaths;
}
