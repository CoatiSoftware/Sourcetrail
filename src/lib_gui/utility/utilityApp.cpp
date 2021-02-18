#include "utilityApp.h"

#include <chrono>
#include <mutex>
#include <set>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/read.hpp>
#include <boost/process.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/process/child.hpp>
//#include <boost/process/env.hpp>
//#include <boost/process/environment.hpp>
#include <boost/process/io.hpp>
#include <boost/process/search_path.hpp>
//#include <boost/process/shell.hpp>
#include <boost/process/start_dir.hpp>
//#include <boost/process/system.hpp>

#include <boost/filesystem/path.hpp>
#include <iostream>

#include <QProcess>
#include <QRegularExpression>
#include <QSysInfo>
#include <QThread>
#include <qprocessordetection.h>

#include "AppPath.h"
#include "ApplicationSettings.h"
#include "ScopedFunctor.h"
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
std::set<std::shared_ptr<boost::process::child>> s_runningBoostProcesses;
}	 // namespace utility

std::wstring utility::searchPath(const std::wstring& bin, bool& ok)
{
	ok = false;
	std::wstring r = boost::process::search_path(bin).generic_wstring();
	if (!r.empty())
	{
		ok = true;
		return r;
	}
	return bin;
}

std::wstring utility::searchPath(const std::wstring& bin)
{
	bool ok;
	return searchPath(bin, ok);
}

utility::ProcessOutput utility::executeProcessBoost2(
	const std::wstring& command,
	const std::vector<std::wstring>& arguments,
	const FilePath& workingDirectory,
	const int timeout,
	bool logProcessOutput)
{
	std::string output = "";
	int exitCode = 103;
	try
	{
		exitCode = 104;
		std::cout << "exitCode: " << exitCode << std::endl;

		boost::asio::io_service ios;
		boost::process::async_pipe ap(ios);
		exitCode = 105;
		std::cout << "exitCode: " << exitCode << std::endl;

		std::shared_ptr<boost::process::child> process;

		boost::process::environment env = boost::this_process::environment();
		std::vector<std::string> previousPath = env["PATH"].to_vector();
		env["PATH"] = {"/opt/local/bin", "/usr/local/bin", "$HOME/bin"};
		for (const std::string& entry: previousPath)
		{
			env["PATH"].append(entry);
		}

		if (workingDirectory.empty())
		{
			process = std::make_shared<boost::process::child>(
				searchPath(command),
				boost::process::args(arguments),
				env,
				boost::process::std_in.close(),
				(boost::process::std_out & boost::process::std_err) > ap);
		}
		else
		{
			process = std::make_shared<boost::process::child>(
				searchPath(command),
				boost::process::args(arguments),
				boost::process::start_dir(workingDirectory.wstr()),
				env,
				boost::process::std_in.close(),
				(boost::process::std_out & boost::process::std_err) > ap);
		}
		exitCode = 106;
		std::cout << "exitCode: " << exitCode << std::endl;

		{
			std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
			s_runningBoostProcesses.insert(process);
		}

		ScopedFunctor remover([process]() {
			std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
			s_runningBoostProcesses.erase(process);
		});

		exitCode = 107;
		std::cout << "exitCode: " << exitCode << std::endl;

		std::vector<char> buf(128);
		auto stdOutBuffer = boost::asio::buffer(buf);
		std::string logBuffer;
		exitCode = 108;
		std::cout << "exitCode: " << exitCode << std::endl;

		std::function<void(const boost::system::error_code& ec, std::size_t n)> onStdOut =
			[&output, &buf, &stdOutBuffer, &ap, &onStdOut, &logBuffer, logProcessOutput](
				const boost::system::error_code& ec, std::size_t size) {
				std::string text;
				text.reserve(size);
				text.insert(text.end(), buf.begin(), buf.begin() + size);

				output += text;
				if (logProcessOutput)
				{
					logBuffer += text;
					const bool isEndOfLine = (logBuffer.back() == '\n');
					const std::vector<std::string> lines = utility::splitToVector(logBuffer, "\n");
					for (size_t i = 0; i < lines.size() - (isEndOfLine ? 0 : 1); i++)
					{
						LOG_INFO_BARE("Process output: " + lines[i]);
					}
					if (isEndOfLine)
					{
						logBuffer.clear();
					}
					else
					{
						logBuffer = lines.back();
					}
				}
				if (!ec)
				{
					boost::asio::async_read(ap, stdOutBuffer, onStdOut);
				}
			};

		exitCode = 109;
		std::cout << "exitCode: " << exitCode << std::endl;
		boost::asio::async_read(ap, stdOutBuffer, onStdOut);
		exitCode = 110;
		std::cout << "exitCode: " << exitCode << std::endl;
		ios.run();
		exitCode = 111;
		std::cout << "exitCode: " << exitCode << std::endl;

		if (timeout > 0)
		{
			if (!process->wait_for(std::chrono::milliseconds(timeout)))
			{
				process->terminate();
			}
		}
		else
		{
			process->wait();
		}
		exitCode = 112;
		std::cout << "exitCode: " << exitCode << std::endl;

		if (logProcessOutput)
		{
			for (const std::string& line: utility::splitToVector(logBuffer, "\n"))
			{
				LOG_INFO_BARE("Process output: " + line);
			}
		}

		exitCode = process->exit_code();
	}
	catch (const boost::process::process_error& e)
	{
		ProcessOutput ret;
		ret.error = utility::decodeFromUtf8(e.code().message());
		ret.exitCode = e.code().value();
		LOG_ERROR_BARE(L"Process error: " + ret.error);

		return ret;
	}
	catch (...)
	{
		exitCode = 114;
		std::cout << "exitCode: " << exitCode << std::endl;
	}

	ProcessOutput ret;
	ret.output = utility::trim(utility::decodeFromUtf8(output));
	ret.exitCode = exitCode;
	return ret;
}

std::string utility::executeProcessUntilNoOutput(
	const std::wstring& commandPath,
	const std::vector<std::wstring>& commandArguments,
	const FilePath& workingDirectory,
	const int waitTime)
{
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);

	if (!workingDirectory.empty())
	{
		process.setWorkingDirectory(QString::fromStdWString(workingDirectory.wstr()));
	}

	QString command = QString::fromStdWString(commandPath);
	for (const std::wstring& commandArgument: commandArguments)
	{
		command += QString::fromStdWString(L" " + commandArgument);
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

void utility::killRunningProcesses()
{
	std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
	for (QProcess* process: s_runningProcesses)
	{
		process->kill();
	}
	for (std::shared_ptr<boost::process::child> process: s_runningBoostProcesses)
	{
		process->terminate();
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
