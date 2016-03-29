#include "utility/headerSearch/VisualStudioDetector.h"

#include <QSettings>

#include <string>
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/headerSearch/StandardHeaderDetection.h"

VisualStudioDetector::~VisualStudioDetector()
{

}

std::vector<FilePath> VisualStudioDetector::getStandardHeaderPaths()
{
	QSettings registry;
	std::string vstoolstring = m_name + "COMNTOOLS";
	std::vector<FilePath> path;
	if( const char* vs_env = std::getenv(vstoolstring.c_str()))
	{
		FilePath VSHeaderPath(vs_env);
		VSHeaderPath = VSHeaderPath.concat("../VC/include");
		if(VSHeaderPath.exists())
		{
			LOG_INFO_STREAM(<< "Visual Studio 20" << m_name.substr(2,m_name.length()-3) << " includes detected");
			path.push_back(VSHeaderPath.str());
		}
	}
	return path;
}

// Add Visual Studio Version to the available detectors
static StandardHeaderDetection::Add<VisualStudioDetector> vs2015("VS140");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2013("VS120");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2012("VS110");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2010("VS90");

