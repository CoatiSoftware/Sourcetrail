#include "qt/window/QtAbout.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>

#include "version.h"

QtAbout::QtAbout(QWidget *parent)
	: QtSettingsWindow(parent)
{
	raise();
}

QSize QtAbout::sizeHint() const
{
	return QSize(500,500);
}

void QtAbout::setup()
{
	setupForm();

	updateTitle("About Coati");
	updateDoneButton("Ok");
	hideCancelButton(true);
}

void QtAbout::populateForm(QFormLayout* layout)
{
	QLabel* developerLabel = new QLabel(
		"\nManuel Dobusch\n"
		"Eberhard Gräther\n"
		"Malte Langkabel\n"
		"Victoria Pfausler\n"
		"Andreas Stallinger\n");

	layout->addRow(QString("Developed by:"), developerLabel);
	layout->addRow(QString("Version:"), new QLabel(GIT_VERSION_NUMBER));
	layout->addRow(QString("Branch:"), new QLabel(GIT_BRANCH));
	layout->addRow(QString("Buildtype:"), new QLabel(BUILD_TYPE));
	layout->addRow(QString("Date:"), new QLabel(GIT_COMMIT_TIME));
}

void QtAbout::handleCancelButtonPress()
{
	emit canceled();
}

void QtAbout::handleUpdateButtonPress()
{
	close();
}

void QtAbout::handleDoneButtonPress()
{
	close();
}
