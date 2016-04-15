#include "utility/headerSearch/VisualStudioDetector.h"

#include <string>

#include <QSettings>
#include <QSysInfo>

#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"

VisualStudioDetector::VisualStudioDetector(const std::string name, int version, bool isExpress)
	: DetectorBase(name)
	, m_version(version)
	, m_isExpress(isExpress)
{
}

VisualStudioDetector::~VisualStudioDetector()
{
}

std::vector<FilePath> VisualStudioDetector::getStandardHeaderPaths()
{
	FilePath vsInstallPath = getVsInstallPathUsingRegistry();

	// vc++ headers
	std::vector<FilePath> headerPaths;
	if (vsInstallPath.exists())
	{
		std::vector<std::string> subdirectories;
		subdirectories.push_back("vc/include");
		subdirectories.push_back("vc/atlmfc/include");

		for (size_t i = 0; i < subdirectories.size(); i++)
		{
			FilePath headerSearchPath = vsInstallPath.concat(subdirectories[i]);
			if (headerSearchPath.exists())
			{
				headerPaths.push_back(headerSearchPath.canonical().str());
			}
		}
	}

	if (headerPaths.size() > 0)
	{
		//windows sdk
		std::vector<std::string> windowsSdkVersions;
		windowsSdkVersions.push_back("v8.1A");
		windowsSdkVersions.push_back("v8.0A");
		windowsSdkVersions.push_back("v7.1A");
		windowsSdkVersions.push_back("v7.0A");

		for (size_t i = 0; i < windowsSdkVersions.size(); i++)
		{
			FilePath sdkPath = getWindowsSdkPathUsingRegistry(windowsSdkVersions[i]);
			if (sdkPath.exists())
			{
				FilePath sdkIncludePath = sdkPath.concat("include/");
				if (sdkIncludePath.exists())
				{
					std::vector<std::string> subdirectories;
					subdirectories.push_back("shared");
					subdirectories.push_back("um");
					subdirectories.push_back("winrt");

					bool usingSubdirectories = false;
					for (size_t j = 0; j < subdirectories.size(); j++)
					{
						FilePath sdkSubdirectory = sdkPath.concat(subdirectories[j]);
						if (sdkSubdirectory.exists())
						{
							headerPaths.push_back(sdkSubdirectory);
							usingSubdirectories = true;
						}
					}

					if (!usingSubdirectories)
					{
						headerPaths.push_back(sdkIncludePath);
					}
					break;
				}
			}
		}
	}
	return headerPaths;
}

std::vector<FilePath> VisualStudioDetector::getStandardFrameworkPaths()
{
	return std::vector<FilePath>();
}

FilePath VisualStudioDetector::getVsInstallPathUsingRegistry()
{
	QString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\";
	if (QSysInfo::currentCpuArchitecture() == "x86_64")
	{
		key += "Wow6432Node\\";
	}
	key += "Microsoft\\";
	key += (m_isExpress ? "VCExpress" : "VisualStudio");
	key += "\\" + QString::number(m_version) + ".0";

	QSettings expressKey(key, QSettings::NativeFormat); // NativeFormat means from Registry on Windows.
	QString value = expressKey.value("InstallDir").toString() + "../../";

	FilePath path(value.toStdString());
	if (path.exists())
	{
		return path;
	}

	return FilePath();
}

FilePath VisualStudioDetector::getWindowsSdkPathUsingRegistry(const std::string& version)
{
	QString key(("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\" + version).c_str());

	QSettings expressKey(key, QSettings::NativeFormat); // NativeFormat means from Registry on Windows.
	QString value = expressKey.value("InstallationFolder").toString();

	FilePath path(value.toStdString());
	if (path.exists())
	{
		return path;
	}

	return FilePath();
}
