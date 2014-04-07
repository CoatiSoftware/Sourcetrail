#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <memory>
#include <vector>

#include "utility/logging/Logger.h"

class LogManager
{
public:
	static std::shared_ptr<LogManager> getInstance();
	static void destroyInstance();

	~LogManager();

	void addLogger(std::shared_ptr<Logger> logger);
	void removeLogger(std::shared_ptr<Logger> logger);
	void removeLoggersByType(const std::string& type);
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

	LogManager();
	LogManager(const LogManager&);
	void operator=(const LogManager&);

	tm getTime();

	std::vector<std::shared_ptr<Logger> > m_loggers;
};

#endif // LOG_MANAGER_H
