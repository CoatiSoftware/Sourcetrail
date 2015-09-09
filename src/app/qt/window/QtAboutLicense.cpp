#include "qt/window/QtAboutLicense.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTextBrowser>
#include <QTextBlock>
#include <QTextEdit>

#include "licenses.h"

QtAboutLicense::QtAboutLicense(QWidget *parent)
	: QtSettingsWindow(parent)
{
	raise();
}

QSize QtAboutLicense::sizeHint() const
{
	return QSize(600,600);
}

void QtAboutLicense::setup()
{
	setupForm();

	updateTitle("3rd Party Licenses");
	updateDoneButton("Ok");
	hideCancelButton(true);
}

void QtAboutLicense::populateForm(QFormLayout* layout)
{
	for(ThirdPartyLicense license : licenses3rdParties)
	{
		QLabel* licenseName = new QLabel();
		licenseName->setText( QString::fromLatin1(license.name));
		QFont _font = licenseName->font();
		_font.setPixelSize(36);
		licenseName->setFont(_font);
		layout->addWidget(licenseName);

		QLabel* licenseURL = new QLabel();
		licenseURL->setText(QString::fromLatin1("(<a href=\"%1\">Website</a>)")
									.arg(QString::fromLatin1(license.url)));
		licenseURL->setOpenExternalLinks(true);
		layout->addWidget(licenseURL);

		QLabel* licenseText = new QLabel();
		licenseText->setFixedWidth(450);
		licenseText->setWordWrap(true);
		licenseText->setText(QString::fromLatin1(license.license));
		layout->addWidget(licenseText);
	}
}

void QtAboutLicense::handleCancelButtonPress()
{
	emit canceled();
}

void QtAboutLicense::handleUpdateButtonPress()
{
	close();
}

void QtAboutLicense::handleDoneButtonPress()
{
	close();
}
