#ifndef LOG_CONTROLLER_H
#define LOG_CONTROLLER_H

#include "component/controller/Controller.h"
#include "component/view/LogView.h"

class StorageAccess;

class LogController
	: public Controller
{
public:
	LogController(StorageAccess* storageAccess);
	~LogController();

private:
	LogView* getView() const;

	virtual void clear();

	StorageAccess* m_storageAccess;
};

#endif // LOG_CONTROLLER_H
