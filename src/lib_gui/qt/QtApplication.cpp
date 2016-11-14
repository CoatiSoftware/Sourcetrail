#include "qt/QtApplication.h"

#include <QFileOpenEvent>

#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/logging/LogManager.h"
#include "settings/ApplicationSettings.h"

QtApplication::QtApplication(int& argc, char** argv)
	: QApplication(argc, argv)
{
	connect(this, &QCoreApplication::aboutToQuit,
		[=]()
		{
			LogManager::getInstance()->setLoggingEnabled(false);
		});

}

int QtApplication::exec()
{
	LogManager::getInstance()->setLoggingEnabled(ApplicationSettings::getInstance()->getLoggingEnabled());

	return QCoreApplication::exec();
}

// responds to FileOpenEvent specific for Mac
bool QtApplication::event(QEvent *event)
{
	if (event->type() == QEvent::FileOpen)
	{
		QFileOpenEvent* fileEvent = dynamic_cast<QFileOpenEvent*>(event);

		FilePath path(fileEvent->file().toStdString());

		if (path.exists() && path.extension() == ".coatiproject")
		{
			MessageDispatchWhenLicenseValid(
				std::make_shared<MessageLoadProject>(path.str(), false)
			).dispatch();
			return true;
		}
	}

	return QApplication::event(event);
}
