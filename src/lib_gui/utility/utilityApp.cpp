#include "utility/utilityApp.h"

#include <QProcess>
#include <QSysInfo>
#include <QThread>
#include <qprocessordetection.h>

#include "utility/utilityString.h"

namespace utility
{
	std::mutex s_runningProcessesMutex;
	std::set<QProcess*> s_runningProcesses;
}

std::string utility::executeProcess(const std::string& command, const std::string& workingDirectory, int timeout)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(workingDirectory.c_str());
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

	std::string processoutput = process.readAll().toStdString();
	process.close();
	processoutput = utility::trim(processoutput);

	return processoutput;
}

int utility::executeProcessAndGetExitCode(const std::string& command, const std::string& workingDirectory, int timeout)
{
	QProcess process;

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(workingDirectory.c_str());
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
	return QThread::idealThreadCount();
}
