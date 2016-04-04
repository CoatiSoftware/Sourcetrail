#include "utility/headerSearch/VisualStudioDetector.h"

#include <QSettings>
#include <QSysInfo>
#include <QDir>

#include <string>
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/headerSearch/StandardHeaderDetection.h"
#include <iostream>

VisualStudioDetector::VisualStudioDetector(const std::string name)
	: DetectorBase("")
	, m_isExpress(false)
{
	setName(name);
}

VisualStudioDetector::~VisualStudioDetector()
{
}

void VisualStudioDetector::setName(const std::string& version)
{
	m_versionNumber = std::stoi(version);
	if (m_versionNumber > 8 && m_versionNumber < 15)
	{
		DetectorBase::setName("VS" + version + "0");
	}
	else
	{
		// unsupported Visual Studio version
	}
}

std::string VisualStudioDetector::getFullName()
{
	return "Visual Studio " + std::to_string(m_versionNumber + 1) + (m_isExpress ? " Express" : "");
}

std::vector<FilePath> VisualStudioDetector::getStandardHeaderPaths()
{
	std::vector<FilePath> headerPaths;
	// vc++ headers
	if ( !getStanardHeaderPathsUsingEnvironmentVariable(headerPaths) )
	{
		if ( !getStanardHeaderPathsUsingRegistry(headerPaths) )
		{
			if ( !getStanardHeaderPathsUsingRegistry(headerPaths, true) )
			{
				return headerPaths;
			}
		}
	}

	//windows sdk
	//TODO


	return headerPaths;
}

std::string getInstallDir(const std::string RegistryKey)
{
	return "";
}

std::string getWindowsSDKDir()
{
	return "";
}

bool VisualStudioDetector::getStanardHeaderPathsUsingEnvironmentVariable(std::vector<FilePath>& paths)
{
	std::string VSToolstring = m_name + "comntools";
	std::vector<FilePath> path;

	if ( const char* vs_env = std::getenv(VSToolstring.c_str()))
	{
		FilePath VSHeaderpath(vs_env);
		VSHeaderpath = VSHeaderpath.concat("../vc/include");
		if (VSHeaderpath.exists())
		{
			LOG_INFO_STREAM(<< getFullName() << " includes detected");
			path.push_back(VSHeaderpath.str());
			paths = std::move(path);
			return true;
		}
	}
	return false;
}

bool VisualStudioDetector::getStanardHeaderPathsUsingRegistry(std::vector<FilePath>& paths, bool lookForExpressVersion)
{
	QString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\";
	if (QSysInfo::currentCpuArchitecture() == "x86_64")
	{
		key += "Wow6432Node\\";
	}
	key += "Microsoft\\";
	key += ( lookForExpressVersion ? "VCExpress" : "VisualStudio" );
	key += "\\" + QString::number(m_versionNumber) + ".0";

	QSettings expressKey(key, QSettings::NativeFormat);
	QString value = expressKey.value("InstallDir").toString() + "../VC/include";
	QDir dir(value);
	if ( dir.exists())
	{
		if ( lookForExpressVersion )
		{
			m_isExpress = true;
		}
		return true;
	}

	return false;
}

// Add Visual Studio Version to the available detectors
static StandardHeaderDetection::Add<VisualStudioDetector> vs2015("14");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2013("12");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2012("11");
static StandardHeaderDetection::Add<VisualStudioDetector> vs2010("9");

