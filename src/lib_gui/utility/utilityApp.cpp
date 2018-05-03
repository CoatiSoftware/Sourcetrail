#include "utility/utilityApp.h"

#include <QProcess>
#include <QSysInfo>
#include <QThread>
#include <qprocessordetection.h>

#include "License.h"
#include "settings/ApplicationSettings.h"
#include "utility/logging/logging.h"
#include "utility/AppPath.h"
#include "utility/UserPaths.h"
#include "utility/utilityString.h"

namespace utility
{
	std::mutex s_runningProcessesMutex;
	std::set<QProcess*> s_runningProcesses;
}

std::string utility::executeProcess(const std::string& command, const FilePath& workingDirectory, const int timeout)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		process.start(command.c_str());
		s_runningProcesses.insert(&process);
	}

	process.waitForFinished(timeout);
	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		s_runningProcesses.erase(&process);
	}

	// QProcess::ProcessError error = process.error();

	std::string processoutput = process.readAll().toStdString();
	process.close();
	processoutput = utility::trim(processoutput);

	return processoutput;
}

std::string utility::executeProcessUntilNoOutput(const std::string& command, const FilePath& workingDirectory, const int waitTime)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		process.start(command.c_str());
		s_runningProcesses.insert(&process);
	}

	std::string processoutput = "";
	while (!process.waitForFinished(waitTime))
	{
		const std::string currentOutput = process.readAll().toStdString();
		if (currentOutput.empty())
		{
			LOG_WARNING("Canceling process because it did not generate any output during the last " + std::to_string(waitTime / 1000) + " seconds.");
			break;
		}
		else
		{
			processoutput += currentOutput;
		}
	}

	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		s_runningProcesses.erase(&process);
	}

	processoutput += process.readAll().toStdString();
	process.close();
	processoutput = utility::trim(processoutput);

	return processoutput;
}

int utility::executeProcessAndGetExitCode(
	const std::wstring& commandPath,
	const std::vector<std::wstring>& commandArguments,
	const FilePath& workingDirectory,
	const int timeout
){
	QProcess process;

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	QString command = QString::fromStdWString(commandPath);
	for (const std::wstring& commandArgument : commandArguments)
	{
		command += " " + QString::fromStdWString(commandArgument);
	}

	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		process.start(command);
		s_runningProcesses.insert(&process);
	}

	process.waitForFinished(timeout);
	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		s_runningProcesses.erase(&process);
	}

	int exitCode = process.exitCode();
	process.close();
	return exitCode;
}

void utility::killRunningProcesses()
{
	std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
	for (QProcess* process : s_runningProcesses)
	{
		process->kill();
	}
}

OsType utility::getOsType()
{
	if (QSysInfo::windowsVersion() != QSysInfo::WV_None)
	{
		return OS_WINDOWS;
	}
	else if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		return OS_MAC;
	}
	else
	{
		return OS_LINUX;
	}
	return OS_UNKNOWN;
}

int utility::getIdealThreadCount()
{
	int threadCount = QThread::idealThreadCount();
	if (getOsType() == OS_WINDOWS)
	{
		threadCount -= 1;
	}
	return std::max(1, threadCount);
}

bool utility::saveLicense(const License* license)
{
	if (license == nullptr)
	{
		return false;
	}

	if (license->isValid())
	{
		ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
		if (appSettings == nullptr)
		{
			LOG_ERROR_STREAM(<< "Unable to retrieve app settings");
			return false;
		}

		const FilePath appLocation = AppPath::getAppPath();
		appSettings->setLicenseString(license->getLicenseEncodedString(appLocation.str()));
		appSettings->setLicenseCheck(license->hashLocation(appLocation.getAbsolute().str()));
		appSettings->save();
		return true;
	}
	else
	{
		LOG_ERROR( "The entered license key is invalid.");
		return false;
	}
}
