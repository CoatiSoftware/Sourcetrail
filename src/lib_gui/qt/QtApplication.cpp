#include "qt/QtApplication.h"

#include <QFileOpenEvent>

#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/logging/LogManager.h"
#include "component/controller/LogController.h"
#include "settings/ApplicationSettings.h"

QtApplication::QtApplication(int& argc, char** argv)
	: QApplication(argc, argv)
{
}

int QtApplication::exec()
{
	LogController* log = dynamic_cast<LogController*>(LogManager::getInstance()->getLoggerByType("WindowLogger"));
	if (log != nullptr)
	{
		log->setEnabled(true);
	}

	return QApplication::exec();
}

// responds to FileOpenEvent specific for Mac
bool QtApplication::event(QEvent *event)
{
	if (event->type() == QEvent::FileOpen)
	{
		QFileOpenEvent* fileEvent = dynamic_cast<QFileOpenEvent*>(event);

		FilePath path(fileEvent->file().toStdString());

		if (path.exists() && (path.extension() == ".srctrlprj" || path.extension() == ".coatiproject"))
		{
			MessageLoadProject(path.str(), false).dispatch();
			return true;
		}
	}

	return QApplication::event(event);
}
