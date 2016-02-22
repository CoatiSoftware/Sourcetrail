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
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "utility/AppPath.h"
#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/ResourcePaths.h"

QtLicense::QtLicense(QWidget *parent)
	: QtSettingsWindow(parent, 68)
{
	raise();
}

QSize QtLicense::sizeHint() const
{
	return QSize(725, 550);
}

void QtLicense::clear()
{
	if (m_licenseText)
	{
		m_licenseText->setText("");
	}

	if (m_errorLabel)
	{
		m_errorLabel->setText(" ");
	}
}

void QtLicense::load()
{
	clear();

	License license;
	bool isLoaded = license.loadFromEncodedString(
		ApplicationSettings::getInstance()->getLicenseString(), AppPath::getAppPath());
	if (!isLoaded)
	{
		return;
	}

	if (m_licenseText)
	{
		m_licenseText->setText(license.getLicenseString().c_str());
	}
}

void QtLicense::setup()
{
	setStyleSheet((
		utility::getStyleSheet(ResourcePaths::getGuiPath() + "setting_window/window.css") +
		utility::getStyleSheet(ResourcePaths::getGuiPath() + "license/license.css")
	).c_str());

	addLogo();

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(25, 100, 25, 30);

	QVBoxLayout* subLayout = new QVBoxLayout();
	subLayout->setContentsMargins(200, 0, 0, 0);

	QLabel* licenseName = new QLabel(this);
	licenseName->setText(QString::fromLatin1("Enter Licence"));
	licenseName->setObjectName("titleLabel");
	subLayout->addWidget(licenseName);

	subLayout->addSpacing(10);

	QLabel* licenseIntro = new QLabel(this);
	licenseIntro->setText(QString::fromLatin1("Please enter a licence key to activate Coati:"));
	licenseIntro->setObjectName("licenseIntro");
	subLayout->addWidget(licenseIntro);

	m_licenseText = new QTextEdit(this);
	m_licenseText->setObjectName("licenseField");
	m_licenseText->setPlaceholderText(
		"-----BEGIN LICENSE-----\n"
		"Jane Doe\n"
		"Single User License\n"
		"Coati 0\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxx\n"
		"-----END LICENSE-----\n"
	);
	subLayout->addWidget(m_licenseText);

	m_errorLabel = new QLabel(this);
	m_errorLabel->setObjectName("licenseError");
	m_errorLabel->setText(" ");
	subLayout->addWidget(m_errorLabel);

	subLayout->addSpacing(10);

	QLabel* linkLabel = new QLabel(this);
	linkLabel->setObjectName("linkLabel");
	linkLabel->setText("<a href=\"https://coati.io/buy-license\">Don't have a license key yet?</a>");
	linkLabel->setOpenExternalLinks(true);
	linkLabel->setGeometry(275, 300, 300, 50);
	subLayout->addWidget(linkLabel);

	layout->addLayout(subLayout);
	layout->addSpacing(20);

	showButtons(layout);
	updateDoneButton("Activate");

	resize(725, 550);
}

void QtLicense::handleCancelButtonPress()
{
	emit canceled();
}

void QtLicense::handleUpdateButtonPress()
{
	std::string licenseString = m_licenseText->toPlainText().toStdString();
	if (licenseString.size() == 0)
	{
		m_errorLabel->setText("No licence key was entered.");
		return;
	}

	License license;
	bool isLoaded = license.loadFromString(licenseString);
	if (!isLoaded)
	{
		m_errorLabel->setText("The entered license key is malformed.");
		return;
	}

	license.loadPublicKeyFromString(PublicKey);
	license.print();

	if (license.isValid())
	{
		ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
		std::string appLocation = AppPath::getAppPath();
		appSettings->setLicenseString(license.getLicenseEncodedString(appLocation));
		FilePath p(appLocation);
		appSettings->setLicenseCheck(license.hashLocation(p.absolute().str()));
		appSettings->save();

		MessageEnteredLicense().dispatch();
	}
	else
	{
		m_errorLabel->setText("The entered license key is invalid.");
		return;
	}

	m_errorLabel->setText(" ");
	setCancelAble(true);

	emit finished();
}
