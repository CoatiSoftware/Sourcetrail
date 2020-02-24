#include "CombinedPathDetector.h"

#include "FilePath.h"

CombinedPathDetector::CombinedPathDetector(): PathDetector("combined") {}

void CombinedPathDetector::addDetector(std::shared_ptr<PathDetector> detector)
{
	m_detectors.push_back(detector);
}

std::vector<std::string> CombinedPathDetector::getWorkingDetectorNames()
{
	std::vector<std::string> names;
	for (const std::shared_ptr<PathDetector>& detector: m_detectors)
	{
		if (detector->isWorking())
		{
			names.push_back(detector->getName());
		}
	}
	return names;
}

std::vector<FilePath> CombinedPathDetector::getPathsForDetector(const std::string& detectorName) const
{
	for (const std::shared_ptr<PathDetector>& detector: m_detectors)
	{
		if (detector->getName() == detectorName)
		{
			return detector->getPaths();
		}
	}
	return std::vector<FilePath>();
}

std::vector<FilePath> CombinedPathDetector::doGetPaths() const
{
	for (const std::shared_ptr<PathDetector>& detector: m_detectors)
	{
		std::vector<FilePath> detectedPaths = detector->getPaths();
		if (!detectedPaths.empty())
		{
			return detectedPaths;
		}
	}
	return std::vector<FilePath>();
}
