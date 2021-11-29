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
#include <boost/process/io.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/start_dir.hpp>

#include <QThread>

#include "ScopedFunctor.h"
#include "logging.h"
#include "utilityString.h"

namespace utility
{
std::mutex s_runningProcessesMutex;
std::set<std::shared_ptr<boost::process::child>> s_runningProcesses;
}	 // namespace utility

std::string utility::getDocumentationLink()
{
	return "https://github.com/CoatiSoftware/Sourcetrail/blob/master/DOCUMENTATION.md";
}

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

namespace
{
template <typename Rep, typename Period>
bool safely_wait_for(boost::process::child& process, const std::chrono::duration<Rep, Period>& rel_time)
{
	// This wrapper around boost::process::wait_for handles the following edge case:
	// Calling wait_for on an already exitted process will wait for the entire timeout.
	if (process.running())
	{
		return process.wait_for(rel_time);
	}
	else
	{
		return true;	// The process exitted
	}
}
}	 // namespace

utility::ProcessOutput utility::executeProcess(
	const std::wstring& command,
	const std::vector<std::wstring>& arguments,
	const FilePath& workingDirectory,
	const bool waitUntilNoOutput,
	const int timeout,
	bool logProcessOutput)
{
	std::string output = "";
	int exitCode = 255;
	try
	{
		boost::asio::io_service ios;
		boost::process::async_pipe ap(ios);

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

		{
			std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
			s_runningProcesses.insert(process);
		}

		ScopedFunctor remover(
			[process]()
			{
				std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
				s_runningProcesses.erase(process);
			});

		bool outputReceived = false;
		std::vector<char> buf(128);
		auto stdOutBuffer = boost::asio::buffer(buf);
		std::string logBuffer;

		std::function<void(const boost::system::error_code& ec, std::size_t n)> onStdOut =
			[&output, &buf, &stdOutBuffer, &ap, &onStdOut, &outputReceived, &logBuffer, logProcessOutput](
				const boost::system::error_code& ec, std::size_t size)
		{
			std::string text;
			text.reserve(size);
			text.insert(text.end(), buf.begin(), buf.begin() + size);

			if (!text.empty())
			{
				outputReceived = true;
			}

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

		boost::asio::async_read(ap, stdOutBuffer, onStdOut);
		ios.run();

		if (timeout > 0)
		{
			if (waitUntilNoOutput)
			{
				while (!safely_wait_for(*process, std::chrono::milliseconds(timeout)))
				{
					if (!outputReceived)
					{
						LOG_WARNING(
							"Canceling process because it did not generate any output during the "
							"last " +
							std::to_string(timeout / 1000) + " seconds.");
						process->terminate();
						break;
					}
					outputReceived = false;
				}
			}
			else
			{
				if (!safely_wait_for(*process, std::chrono::milliseconds(timeout)))
				{
					LOG_WARNING(
						"Canceling process because it timed out after " +
						std::to_string(timeout / 1000) + " seconds.");
					process->terminate();
				}
			}
		}
		else
		{
			process->wait();
		}

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

	ProcessOutput ret;
	ret.output = utility::trim(utility::decodeFromUtf8(output));
	ret.exitCode = exitCode;
	return ret;
}

void utility::killRunningProcesses()
{
	std::lock_guard<std::mutex> lock(s_runningProcessesMutex);
	for (std::shared_ptr<boost::process::child> process: s_runningProcesses)
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
