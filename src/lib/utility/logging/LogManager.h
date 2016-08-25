#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <memory>
#include <mutex>

#include "utility/logging/Logger.h"
#include "utility/logging/LogManagerImplementation.h"

class LogManager
{
public:
	static std::shared_ptr<LogManager> getInstance();
	static void destroyInstance();

	~LogManager();

	void addLogger(std::shared_ptr<Logger> logger);
	void removeLogger(std::shared_ptr<Logger> logger);
	void removeLoggersByType(const std::string& type);
	void clearLoggers();
	int getLoggerCount() const;

	void logInfo(
		const std::string& message,
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
	void logError(
		const std::string& message,
		const std::string& file,
		const std::string& function,
		const unsigned int line
	);

private:
	static std::shared_ptr<LogManager> s_instance;
	static std::mutex s_instanceMutex;

	LogManager();
	LogManager(const LogManager&);
	void operator=(const LogManager&);

	LogManagerImplementation m_logManagerImplementation;
};

#endif // LOG_MANAGER_H
