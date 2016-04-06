#include "utility/headerSearch/StandardHeaderDetection.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "utility/file/FilePath.h"
#include "utility/headerSearch/CompilerDetector.h"
#include "utility/headerSearch/VisualStudioDetector.h"
#include "utility/logging/logging.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"

StandardHeaderDetection::DetectorMap StandardHeaderDetection::s_availableDetectors;
StandardHeaderDetection::DetectorMap StandardHeaderDetection::s_detectedCompilers;

StandardHeaderDetection::StandardHeaderDetection()
{
	if (!s_availableDetectors.size())
	{
		addDetector(std::make_shared<CompilerDetector>("gcc"));
		addDetector(std::make_shared<CompilerDetector>("clang"));

		addDetector(std::make_shared<VisualStudioDetector>("14"));
		addDetector(std::make_shared<VisualStudioDetector>("12"));
		addDetector(std::make_shared<VisualStudioDetector>("11"));
		addDetector(std::make_shared<VisualStudioDetector>("9"));

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
	for ( DetectorPair detector : s_availableDetectors)
	{
		if ( detector.second->detect() )
		{
			s_detectedCompilers.insert(detector);
		}
	}
}

std::vector<std::string> StandardHeaderDetection::getDetectedCompilers()
{
    std::vector<std::string> v;
	for ( DetectorPair detector : s_detectedCompilers)
	{
        v.push_back(detector.first);
	}
    return v;
}

std::vector<FilePath> StandardHeaderDetection::getStandardHeaderPaths(std::string compiler)
{
    auto it = s_detectedCompilers.find(compiler);
    if (it != s_detectedCompilers.end())
    {
        return it->second->getStandardHeaderPaths();
    }
    return std::vector<FilePath>();
}

std::vector<FilePath> StandardHeaderDetection::getStandardFrameworkPaths(std::string compiler)
{
    auto it = s_detectedCompilers.find(compiler);
    if (it != s_detectedCompilers.end())
    {
        return it->second->getStandardFrameworkPaths();
    }
    return std::vector<FilePath>();
}

void StandardHeaderDetection::printDetectedCompilers()
{
	std::cout << "Detected Compilers: " << std::endl;

	std::vector<std::string> compilers = getDetectedCompilers();
	for ( std::string compiler : compilers)
	{
		std::cout << compiler << std::endl;
	}
}

void StandardHeaderDetection::printAvailableDetectors()
{
	std::cout << "Available Detectors: " << std::endl;

	for ( DetectorPair detector : s_availableDetectors)
	{
		std::cout << detector.first << std::endl;
	}
}

