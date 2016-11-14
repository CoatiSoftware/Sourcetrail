#include "component/controller/LogController.h"

#include "data/access/StorageAccess.h"
#include "utility/logging/LogManager.h"

LogController::LogController()
	: Logger("WindowLogger")
{
}

LogController::~LogController()
{
}

LogView* LogController::getView() const
{
	return Controller::getView<LogView>();
}

void LogController::clear()
{
	getView()->clear();
}

void LogController::logInfo(const LogMessage& message )
{
	addLog(LOG_INFOS, message);
}

void LogController::logError(const LogMessage& message )
{
	addLog(LOG_ERRORS, message);
}

void LogController::logWarning(const LogMessage& message )
{
	addLog(LOG_WARNINGS, message);
}

void LogController::addLog(Logger::LogLevel type, const LogMessage& message)
{
	getView()->addLog(type, message);
}

