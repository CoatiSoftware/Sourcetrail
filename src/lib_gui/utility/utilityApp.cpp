#include "utilityApp.h"

#include <mutex>
#include <set>

#include <QProcess>
#include <QRegularExpression>
#include <QSysInfo>
#include <QThread>
#include <qprocessordetection.h>

#include "AppPath.h"
#include "ApplicationSettings.h"
#include "UserPaths.h"
#include "logging.h"
#include "utilityString.h"

namespace
{
void logProcessStreams(QProcess& process, std::wstring& outputBuffer, std::wstring& errorBuffer)
{
	{
		outputBuffer += QString(process.readAllStandardOutput()).toStdWString();
		std::vector<std::wstring> outputLines = utility::split<std::vector<std::wstring>>(
			outputBuffer, L"\n");
		for (size_t i = 0; i < outputLines.size() - 1; i++)
		{
			if (outputLines[i].back() == L'\r')
			{
				outputLines[i].pop_back();
			}
			LOG_INFO_BARE(L"Process output: " + outputLines[i]);
		}
		outputBuffer = outputLines.back();
	}
	{
		errorBuffer += QString(process.readAllStandardError()).toStdWString();
		std::vector<std::wstring> errorLines = utility::split<std::vector<std::wstring>>(
			errorBuffer, L"\n");
		for (size_t i = 0; i < errorLines.size() - 1; i++)
		{
			if (errorLines[i].back() == L'\r')
			{
				errorLines[i].pop_back();
			}
			LOG_ERROR_BARE(L"Process error: " + errorLines[i]);
		}
		errorBuffer = errorLines.back();
	}
}
}	 // namespace

namespace utility
{
std::mutex s_runningProcessesMutex;
std::set<QProcess*> s_runningProcesses;
}	 // namespace utility

std::pair<int, std::string> utility::executeProcess(
	const std::string& command, const FilePath& workingDirectory, const int timeout)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QStringList envlist = env.toStringList();
	envlist.replaceInStrings(
		QRegularExpression(QStringLiteral("^(?i)PATH=(.*)")),
		QStringLiteral("PATH=/opt/local/bin:/usr/local/bin:$HOME/bin:\\1"));
	process.setEnvironment(envlist);

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

	const std::string processoutput = process.readAll().toStdString();
	const int exitCode = process.exitCode();
	process.close();

	return std::make_pair(exitCode, utility::trim(processoutput));
}

std::string utility::executeProcessUntilNoOutput(
	const std::string& command, const FilePath& workingDirectory, const int waitTime)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QStringList envlist = env.toStringList();
	envlist.replaceInStrings(
		QRegularExpression(QStringLiteral("^(?i)PATH=(.*)")),
		QStringLiteral("PATH=/opt/local/bin:/usr/local/bin:$HOME/bin:\\1"));
	process.setEnvironment(envlist);

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
			LOG_WARNING(
				"Canceling process because it did not generate any output during the last " +
				std::to_string(waitTime / 1000) + " seconds.");
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
	const int timeout,
	bool logProcessOutput,
	std::wstring* errorMessage)
{
	bool finished = false;

	QProcess process;

	QObject::connect(
		&process,
		&QProcess::errorOccurred,
		[&finished, errorMessage, commandPath](QProcess::ProcessError error) {
			finished = true;
			if (errorMessage != nullptr)
			{
				switch (error)
				{
				case QProcess::FailedToStart:
					*errorMessage = L"File not found or resource error occurred.";
					break;
				case QProcess::Crashed:
					*errorMessage = L"Process crashed.";
					break;
				case QProcess::Timedout:
					*errorMessage = L"Process timed out.";
					break;
				case QProcess::ReadError:
					*errorMessage = L"A read error occurred while executing process.";
					break;
				case QProcess::WriteError:
					*errorMessage = L"A write error occurred while executing process.";
					break;
				case QProcess::UnknownError:
					*errorMessage = L"An unknown error occurred while executing process.";
					break;
				}
			};
		});

	QObject::connect(
		&process,
		static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
		[&finished](int exitCode, QProcess::ExitStatus exitStatus) { finished = true; });


	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	QString command = QString::fromStdWString(commandPath);
	for (const std::wstring& commandArgument: commandArguments)
	{
		command += " " + QString::fromStdWString(commandArgument);
	}

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QStringList envlist = env.toStringList();
	envlist.replaceInStrings(
		QRegularExpression(QStringLiteral("^(?i)PATH=(.*)")),
		QStringLiteral("PATH=/opt/local/bin:/usr/local/bin:$HOME/bin:\\1"));
	process.setEnvironment(envlist);

	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		process.start(command);
		s_runningProcesses.insert(&process);
	}

	{
		std::wstring outputBuffer;
		std::wstring errorBuffer;
		if (timeout == -1)
		{
			while (!finished && !process.waitForFinished(1000))
			{
				if (logProcessOutput)
				{
					logProcessStreams(process, outputBuffer, errorBuffer);
				}
			}
		}
		else
		{
			if (!finished)
			{
				process.waitForFinished(timeout);
			}
		}

		if (logProcessOutput)
		{
			logProcessStreams(process, outputBuffer, errorBuffer);
		}
	}
	{
		std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
		s_runningProcesses.erase(&process);
	}

	const int exitCode = process.exitCode();
	process.close();
	return exitCode;
}

void utility::killRunningProcesses()
{
	std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
	for (QProcess* process: s_runningProcesses)
	{
		process->kill();
	}
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

std::string utility::getOsTypeString()
{
	// WARNING: Don't change these string. The server API relies on them.
	switch (utility::getOsType())
	{
	case OS_WINDOWS:
		return "windows";
	case OS_MAC:
		return "macOS";
	case OS_LINUX:
		return "linux";
	default:
		break;
	}
	return "unknown";
}

ApplicationArchitectureType utility::getApplicationArchitectureType()
{
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64) ||             \
	defined(WIN64)
	return APPLICATION_ARCHITECTURE_X86_64;
#else
	return APPLICATION_ARCHITECTURE_X86_32;
#endif
	return APPLICATION_ARCHITECTURE_UNKNOWN;
}
