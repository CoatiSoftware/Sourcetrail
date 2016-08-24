#include "utility/path_detector/cxx_header/CxxVsHeaderPathDetector.h"

#include <string>

#include <QSettings>
#include <QSysInfo>

#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"

CxxVsHeaderPathDetector::CxxVsHeaderPathDetector(int version, bool isExpress)
	: PathDetector("Visual Studio " + std::to_string(version) + (isExpress ? " Express" : ""))
	, m_version(version)
	, m_isExpress(isExpress)
{
}

CxxVsHeaderPathDetector::~CxxVsHeaderPathDetector()
{
}

std::vector<FilePath> CxxVsHeaderPathDetector::getPaths() const
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

FilePath CxxVsHeaderPathDetector::getVsInstallPathUsingRegistry() const
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

FilePath CxxVsHeaderPathDetector::getWindowsSdkPathUsingRegistry(const std::string& version) const
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
