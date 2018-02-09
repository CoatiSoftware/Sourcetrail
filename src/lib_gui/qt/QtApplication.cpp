#include "qt/QtApplication.h"

#include <QFileOpenEvent>

#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageWindowFocus.h"
#include "utility/logging/LogManager.h"
#include "utility/utilityApp.h"

QtApplication::QtApplication(int& argc, char** argv)
	: QApplication(argc, argv)
{
	connect(this, &QGuiApplication::applicationStateChanged, this, &QtApplication::onApplicationStateChanged);
}

int QtApplication::exec()
{
	return QApplication::exec();
}

// responds to FileOpenEvent specific for Mac
bool QtApplication::event(QEvent *event)
{
	if (event->type() == QEvent::FileOpen)
	{
		QFileOpenEvent* fileEvent = dynamic_cast<QFileOpenEvent*>(event);

		FilePath path(fileEvent->file().toStdWString());

		if (path.exists() && (path.extension() == L".srctrlprj" || path.extension() == L".coatiproject"))
		{
			MessageLoadProject(path).dispatch();
			return true;
		}
	}

	return QApplication::event(event);
}

void QtApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
	MessageWindowFocus(state == Qt::ApplicationActive).dispatch();
}
