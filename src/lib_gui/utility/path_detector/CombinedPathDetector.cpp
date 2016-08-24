#include "utility/path_detector/CombinedPathDetector.h"

CombinedPathDetector::CombinedPathDetector()
	: PathDetector("combined")
{
}

CombinedPathDetector::~CombinedPathDetector()
{
}

void CombinedPathDetector::addDetector(std::shared_ptr<PathDetector> detector)
{
	m_detectors[detector->getName()] = detector;
}

std::vector<std::string> CombinedPathDetector::getWorkingDetectorNames()
{
	std::vector<std::string> names;

	for (DetectorPair detectorEntry: m_detectors)
	{
		if (detectorEntry.second->isWorking())
		{
			names.push_back(detectorEntry.first);
		}
	}

	return names;
}

std::vector<FilePath> CombinedPathDetector::getPaths() const
{
	for (DetectorPair detectorEntry: m_detectors)
	{
		std::vector<FilePath> detectedPaths = detectorEntry.second->getPaths();
		if (!detectedPaths.empty())
		{
			return detectedPaths;
		}
	}
	return std::vector<FilePath>();
}

std::vector<FilePath> CombinedPathDetector::getPaths(std::string detectorName) const
{
	std::vector<FilePath> paths;
	DetectorMap::const_iterator it = m_detectors.find(detectorName);
	if (it != m_detectors.end())
	{
		paths = it->second->getPaths();
	}
	return paths;
}
