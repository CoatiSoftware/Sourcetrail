#include "QtFilesAndDirectoriesDialog.h"

#include <QEvent>
#include <QListView>
#include <QPushButton>

#include "FilePath.h"
#include "utilityApp.h"

QtFilesAndDirectoriesDialog::QtFilesAndDirectoriesDialog(QWidget* parent): QFileDialog(parent)
{
	setFileMode(QFileDialog::Directory);
	setOption(QFileDialog::DontUseNativeDialog, true);
	for (QPushButton* button: findChildren<QPushButton*>())
	{
		if (button->text().toLower().contains(QLatin1String("open")) || button->text().toLower().contains(QLatin1String("choose")))
		{
			button->installEventFilter(this);
			button->disconnect(SIGNAL(clicked()));
			connect(button, &QPushButton::clicked, this, &QtFilesAndDirectoriesDialog::chooseClicked);
			break;
		}
	}
}

QtFilesAndDirectoriesDialog::~QtFilesAndDirectoriesDialog() {}

void QtFilesAndDirectoriesDialog::chooseClicked()
{
	QDialog::accept();
}

bool QtFilesAndDirectoriesDialog::eventFilter(QObject* obj, QEvent* event)
{
	QPushButton* button = qobject_cast<QPushButton*>(obj);
	if (event->type() == QEvent::EnabledChange && button && !button->isEnabled())
	{
		// we need to check if the button is already enabled, otherwise this would cause an infinite loop
		button->setEnabled(true);
	}
	return QObject::eventFilter(obj, event);
}
