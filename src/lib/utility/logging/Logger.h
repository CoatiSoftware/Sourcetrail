#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <vector>

#include "utility/logging/LogMessage.h"

class Logger
{
public:
	Logger(const std::string& type);
	virtual ~Logger();

	std::string getType() const;

	virtual void logInfo(const LogMessage& message) = 0;
	virtual void logWarning(const LogMessage& message) = 0;
	virtual void logError(const LogMessage& message) = 0;

private:
	const std::string m_type;
};

#endif // LOGGER_H
