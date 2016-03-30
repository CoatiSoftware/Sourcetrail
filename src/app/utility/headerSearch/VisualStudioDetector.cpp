#include "utility/headerSearch/VisualStudioDetector.h"

#include <QSettings>

#include <string>
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/headerSearch/StandardHeaderDetection.h"

VisualStudioDetector::VisualStudioDetector(const std::string name)
	: DetectorBase(name)
{
}

VisualStudioDetector::~VisualStudioDetector()
{
}

void VisualStudioDetector::setName(const std::string& name)
{
	if (name.substr(0,2) == "VS")
	{
		m_versionNumber = std::stoi(name.substr(2,name.size()-3).c_str());
		if (m_versionNumber == 0)
		{
			// no version
		}
		DetectorBase::setName(name);
	}
	else
	{
		// invalid Visual Studio detector name
	}
}

std::string VisualStudioDetector::getFullName()
{
	return "Visual Studio " + std::to_string(m_versionNumber + 1);
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
			LOG_INFO_STREAM(<< getFullName() << " includes detected");
			path.push_back(VSHeaderPath.str());
		}
	}
	return path;
}

std::string getInstallDir(const std::string RegistryKey)
{
	return "";
}

std::string getWindowsSDKDir()
{
	return ""; 
}

bool searchForExpress()
{
	QSettings expressKey("HKLM\\SOFTWARE\\Microsoft\\Visual Studio\\", QSettings::NativeFormat);
	return false;
}

bool searchForStandard()
{
	return false;
}

// Add Visual Studio Version to the available detectors
static StandardHeaderDetection::Add<VisualStudioDetector> vs2015("VS140");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2013("VS120");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2012("VS110");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2010("VS90");

