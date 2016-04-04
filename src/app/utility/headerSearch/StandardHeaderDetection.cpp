#include "utility/headerSearch/StandardHeaderDetection.h"

#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"

#include "clang/Driver/ToolChain.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/DriverDiagnostic.h"

typedef std::pair<std::string, std::shared_ptr<DetectorBase>> DetectorPair;

DetectorMap* StandardHeaderDetection::s_availableDetectors;

StandardHeaderDetection::StandardHeaderDetection()
{
	detectHeaders();
}

StandardHeaderDetection::~StandardHeaderDetection()
{
	if ( s_availableDetectors != nullptr )
	{
		delete s_availableDetectors;
	}
}

void StandardHeaderDetection::detectHeaders()
{
	for ( DetectorPair detector : *s_availableDetectors)
	{
		if ( detector.second->detect() )
		{
			m_detectedCompilers.insert(detector);
		}
	}
}

std::vector<std::string> StandardHeaderDetection::getDetectedCompilers()
{
    std::vector<std::string> v;
	for ( DetectorPair detector : m_detectedCompilers)
	{
        v.push_back(detector.first);
	}
    return v;
}

std::vector<FilePath> StandardHeaderDetection::getStandardHeaderPaths(std::string compiler)
{
    auto it = m_detectedCompilers.find(compiler);
    if(it != m_detectedCompilers.end())
    {
        return it->second->getStandardHeaderPaths();
    }
    return std::vector<FilePath>();
}

void StandardHeaderDetection::printdetectedCompilers()
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
	std::cout << "printAvailableDetectors: " << std::endl;

	for ( DetectorPair detector : *s_availableDetectors)
	{
		std::cout << detector.first << std::endl;
	}
}

