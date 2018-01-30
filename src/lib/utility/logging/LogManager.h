#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <memory>

#include "utility/logging/Logger.h"
#include "utility/logging/LogManagerImplementation.h"

class LogManager
{
public:
	static std::shared_ptr<LogManager> getInstance();
	static void destroyInstance();

	~LogManager();

	void setLoggingEnabled(bool enabled);

	void addLogger(std::shared_ptr<Logger> logger);
	void removeLogger(std::shared_ptr<Logger> logger);
	void removeLoggersByType(const std::string& type);
	void clearLoggers();
	int getLoggerCount() const;
	Logger* getLoggerByType(const std::string& type);
	Logger* getLogger(std::shared_ptr<Logger> logger);

	void logInfo(
		const std::string& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);
	void logInfo(
		const std::wstring& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);
	void logWarning(
		const std::string& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);
	void logWarning(
		const std::wstring& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);
	void logError(
		const std::string& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);
	void logError(
		const std::wstring& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);

private:
	static std::shared_ptr<LogManager> s_instance;

	LogManager();
	LogManager(const LogManager&);
	void operator=(const LogManager&);

	LogManagerImplementation m_logManagerImplementation;
	bool m_loggingEnabled;
};

#endif // LOG_MANAGER_H
