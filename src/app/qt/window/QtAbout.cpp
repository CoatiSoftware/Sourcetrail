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
	QLabel* developerName = new QLabel("Developed by:");
	developerName->setAlignment(Qt::AlignTop);
	QLabel* developerLabel = new QLabel(
		"Manuel Dobusch\n"
		"Eberhard Gräther\n"
		"Malte Langkabel\n"
		"Victoria Pfausler\n"
		"Andreas Stallinger\n");
	developerLabel->setAlignment(Qt::AlignTop);
	layout->addRow(developerName, developerLabel);

	QLabel* acknowledgementsName = new QLabel("Acknowledgements:");
	acknowledgementsName->setAlignment(Qt::AlignTop);
	QLabel* acknowledgementsLabel = new QLabel("Coati 0.1 was created in the context of education at the University of Applied Sciences Salzburg\n");
	acknowledgementsLabel->setWordWrap(true);
	acknowledgementsLabel->setAlignment(Qt::AlignTop);
	layout->addRow(acknowledgementsName, acknowledgementsLabel);

	layout->addRow(QString("Version:"), new QLabel(GIT_VERSION_NUMBER));
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
