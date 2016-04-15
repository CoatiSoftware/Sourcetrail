#include "utility/headerSearch/StandardHeaderDetection.h"

#include <cstdlib>
#include <iostream>
#include <utility>

#include "utility/headerSearch/CompilerDetector.h"
#include "utility/headerSearch/VisualStudioDetector.h"
#include "utility/logging/logging.h"

StandardHeaderDetection::DetectorMap StandardHeaderDetection::s_availableDetectors;
StandardHeaderDetection::DetectorMap StandardHeaderDetection::s_workingDetectors;

StandardHeaderDetection::StandardHeaderDetection()
{
	if (!s_availableDetectors.size())
	{
		addDetector(std::make_shared<CompilerDetector>("gcc"));
		addDetector(std::make_shared<CompilerDetector>("clang"));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2010", 9, false));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2010 Express", 9, true));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2012", 11, false));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2012 Express", 11, true));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2013", 12, false));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2013 Express", 12, true));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2015", 14, false));
		addDetector(std::make_shared<VisualStudioDetector>("Visual Studio 2015 Express", 14, true));

		detectHeaders();
	}
}

StandardHeaderDetection::~StandardHeaderDetection()
{
}

void StandardHeaderDetection::addDetector(std::shared_ptr<DetectorBase> detector)
{
	s_availableDetectors.emplace(detector->getName(), detector);
}

void StandardHeaderDetection::detectHeaders()
{
	for (DetectorPair detector: s_availableDetectors)
	{
		if (detector.second->isWorking())
		{
			s_workingDetectors.insert(detector);
		}
	}
}

std::vector<std::string> StandardHeaderDetection::getWorkingDetectorNames()
{
    std::vector<std::string> detectorNames;
	for (DetectorPair detector: s_workingDetectors)
	{
		detectorNames.push_back(detector.first);
	}
    return detectorNames;
}

std::vector<FilePath> StandardHeaderDetection::getStandardHeaderPaths(std::string detectorName)
{
	DetectorMap::const_iterator it = s_workingDetectors.find(detectorName);
    if (it != s_workingDetectors.end())
    {
        return it->second->getStandardHeaderPaths();
    }
    return std::vector<FilePath>();
}

std::vector<FilePath> StandardHeaderDetection::getStandardFrameworkPaths(std::string detectorName)
{
	DetectorMap::const_iterator it = s_workingDetectors.find(detectorName);
    if (it != s_workingDetectors.end())
    {
        return it->second->getStandardFrameworkPaths();
    }
    return std::vector<FilePath>();
}

void StandardHeaderDetection::logAvailableDetectors()
{
	LOG_INFO("Available Header Detectors:");

	for (DetectorPair detector: s_availableDetectors)
	{
		LOG_INFO("Detector: " + detector.first);
	}
}

void StandardHeaderDetection::logWorkingDetectors()
{
	LOG_INFO("Working Header Detectors:");

	for (std::string detectorName: getWorkingDetectorNames())
	{
		LOG_INFO("Detector: " + detectorName);
	}
}

