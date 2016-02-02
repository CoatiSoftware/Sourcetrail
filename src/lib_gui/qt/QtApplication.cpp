#include "qt/QtApplication.h"

#include <QFileOpenEvent>

#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageLoadProject.h"

QtApplication::QtApplication(int& argc, char** argv)
	: QApplication(argc, argv)
{
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
			MessageLoadProject(path.str(), false).dispatch();
			return true;
		}
	}

	return QApplication::event(event);
}
