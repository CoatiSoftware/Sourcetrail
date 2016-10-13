#include "component/controller/LogController.h"

#include "data/access/StorageAccess.h"

LogController::LogController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
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
