#include "qt/window/QtLicense.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTextBrowser>
#include <QTextBlock>
#include <QTextEdit>

#include "License.h"
#include "PublicKey.h"

#include "settings/ApplicationSettings.h"	
#include "utility/logging/logging.h"
#include "utility/file/FilePath.h"

QtLicense::QtLicense(QWidget *parent)
	: QtSettingsWindow(parent)
{
	raise();
}

QSize QtLicense::sizeHint() const
{
	return QSize(600,600);
}

void QtLicense::setup()
{
	setupForm();

	updateTitle("Enter License");
	updateDoneButton("Ok");
}

void QtLicense::populateForm(QFormLayout* layout)
{
	QLabel* licenseName = new QLabel();
	licenseName->setText( QString::fromLatin1("Enter License:"));
	QFont _font = licenseName->font();
	_font.setPixelSize(36);
	licenseName->setFont(_font);
	layout->addWidget(licenseName);

	m_licenseText = new QTextEdit();
	m_licenseText->setMinimumHeight(300);
	layout->addWidget(m_licenseText);
}

void QtLicense::handleCancelButtonPress()
{
	emit canceled();
}

void QtLicense::handleUpdateButtonPress()
{
	License license;
	bool isLoaded = license.loadFromString(m_licenseText->toPlainText().toStdString());
	if(!isLoaded)
	{
		LOG_WARNING("Failed Loading License");
		return;
	}
	license.loadPublicKeyFromString(PublicKey);
	license.print();

	if(license.isValid())
	{
		ApplicationSettings::getInstance()->setLicenseString(license.getLicenseString());
		FilePath p("");
		ApplicationSettings::getInstance()->setLicenseCheck(license.hashLocation(p.absolute().str()));
		ApplicationSettings::getInstance()->save();
		LOG_WARNING("License saved");
	}
	else
	{
		LOG_WARNING("License is not valid");
	}
	emit finished();

}

