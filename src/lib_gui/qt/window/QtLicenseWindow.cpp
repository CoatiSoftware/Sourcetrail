#include "QtLicenseWindow.h"

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QTextEdit>

#include "LicenseChecker.h"
#include "utilityQt.h"
#include "ApplicationSettings.h"
#include "ResourcePaths.h"

QtLicenseWindow::QtLicenseWindow(QWidget *parent)
	: QtWindow(false, parent)
{
}

QSize QtLicenseWindow::sizeHint() const
{
	return QSize(740, 550);
}

void QtLicenseWindow::clear()
{
	if (m_licenseText)
	{
		m_commercialUse->setChecked(false);
		m_privateUse->setChecked(false);

		m_licenseText->setText("");
		m_errorLabel->setText(" ");
	}
}

void QtLicenseWindow::load()
{
	clear();

	std::string licenseString = LicenseChecker::getCurrentLicenseString();

	if (licenseString.size() && m_licenseText)
	{
		m_commercialUse->setChecked(true);

		m_licenseText->setEnabled(true);
		m_licenseText->setText(licenseString.c_str());
	}
	else if (ApplicationSettings::getInstance()->getNonCommercialUse())
	{
		m_privateUse->setChecked(true);
	}
}

void QtLicenseWindow::setErrorMessage(const QString& errorMessage)
{
	if (m_errorLabel)
	{
		m_errorLabel->setText(errorMessage);
	}
}

void QtLicenseWindow::populateWindow(QWidget* widget)
{
	QVBoxLayout* subLayout = new QVBoxLayout();
	subLayout->setContentsMargins(270, 0, 0, 0);

	// commercial
	m_commercialUse = new QRadioButton("Commercial Use");
	m_commercialUse->setObjectName("licenseOption");
	subLayout->addWidget(m_commercialUse);

	QLabel* linkLabel = new QLabel(this);
	linkLabel->setText(
		"For commercial use please <a href=\"http://sourcetrail.com/buy-license\" style=\"color: #007AC2;\">purchase a license</a>, "
		"or get a temporary <a href=\"http://sourcetrail.com/test-license\" style=\"color: #007AC2;\">test license</a>.");
	linkLabel->setOpenExternalLinks(true);
	linkLabel->setWordWrap(true);
	subLayout->addWidget(linkLabel);

	m_licenseText = new QTextEdit();
	m_licenseText->setObjectName("licenseField");
	m_licenseText->setAcceptRichText(false);
	m_licenseText->setPlaceholderText(
		"-----BEGIN LICENSE-----\n"
		"Product: Sourcetrail\n"
		"Licensed to:\n"
		"License type:\n"
		"Valid up to version:\n"
		"-\n"
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
	m_licenseText->setEnabled(false);
	subLayout->addWidget(m_licenseText);

	m_errorLabel = new QLabel(" ", this);
	m_errorLabel->setObjectName("licenseError");
	m_errorLabel->setWordWrap(true);
	subLayout->addWidget(m_errorLabel);

	subLayout->addSpacing(10);


	// private
	m_privateUse = new QRadioButton("Non-Commercial Use");
	m_privateUse->setObjectName("licenseOption");
	subLayout->addWidget(m_privateUse);

	QLabel* licenseIntro2 = new QLabel("You confirm that you will use Sourcetrail solely for non-commercial purposes.");
	licenseIntro2->setWordWrap(true);
	subLayout->addWidget(licenseIntro2);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(subLayout);
	layout->addSpacing(20);

	widget->setLayout(layout);


	connect(m_commercialUse, &QRadioButton::toggled, this, &QtLicenseWindow::optionChanged);
	connect(m_privateUse, &QRadioButton::toggled, this, &QtLicenseWindow::optionChanged);
}

void QtLicenseWindow::windowReady()
{
	m_content->setStyleSheet(m_content->styleSheet() +
			utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"license/license.css")).c_str());

	addLogo();

	updateTitle("Select License Option");

	updateNextButton("Activate");
	setNextEnabled(false);

	setPreviousVisible(false);
	updateCloseButton("Cancel");
}

void QtLicenseWindow::handleNext()
{
	if (m_commercialUse->isChecked())
	{
		std::string licenseString = m_licenseText->toPlainText().toStdString();

		LicenseChecker::LicenseState state = LicenseChecker::checkLicenseString(licenseString);

		std::string errorString;

		switch (state)
		{
			case LicenseChecker::LICENSE_EMPTY:
				errorString = "No license key was entered.";
				break;
			case LicenseChecker::LICENSE_MOVED:
			case LicenseChecker::LICENSE_MALFORMED:
				errorString = "The entered license key is malformed.";
				break;
			case LicenseChecker::LICENSE_INVALID:
				errorString = "The entered license key is invalid.";
				break;
			case LicenseChecker::LICENSE_EXPIRED:
				errorString = "The entered license key is expired.";
				break;
			case LicenseChecker::LICENSE_VALID:
			{
				if (LicenseChecker::getLicenseType(licenseString) == MessageEnteredLicense::LICENSE_NON_COMMERCIAL)
				{
					errorString =
						"The entered license key does not permit commercial use. You no longer need a license "
						"key for non-commercial use. Please choose the non-commercial option below.";
					break;
				}

				LicenseChecker::saveCurrentLicenseString(licenseString);
				m_errorLabel->setText(" ");

				ApplicationSettings::getInstance()->setNonCommercialUse(false);
				ApplicationSettings::getInstance()->save();

				emit finished();
				return;
			}
		}

		m_errorLabel->setText(errorString.c_str());
	}
	else if (m_privateUse->isChecked())
	{
		if (!m_confirmWindow)
		{
			m_confirmWindow = new QtNonCommercialInfoWindow();
			m_confirmWindow->setup();

			connect(m_confirmWindow, &QtWindow::canceled,
				[this]()
				{
					m_confirmWindow->hideWindow();
					raise();
				}
			);

			connect(m_confirmWindow, &QtWindow::finished,
				[this]()
				{
					m_confirmWindow->hideWindow();
					raise();

					ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
					appSettings->setLicenseString("");
					appSettings->setNonCommercialUse(true);
					appSettings->save();

					emit finished();
				}
			);
		}

		m_confirmWindow->showWindow();
	}
}

void QtLicenseWindow::optionChanged()
{
	m_licenseText->setEnabled(m_commercialUse->isChecked());
	setNextEnabled(true);
}


QtNonCommercialInfoWindow::QtNonCommercialInfoWindow(QWidget *parent)
	: QtWindow(false, parent)
{
}

QSize QtNonCommercialInfoWindow::sizeHint() const
{
	return QSize(450, 500);
}

void QtNonCommercialInfoWindow::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	QLabel* titleLabel;
	QLabel* textLabel;


	titleLabel = new QLabel("Is your use non-commercial?");
	textLabel = new QLabel(
		"A use is non-commercial only if it is in no manner primarily intended for or directed toward "
		"commercial advantage or private monetary compensation.");

	QFont font = titleLabel->font();
	font.setBold(true);
	titleLabel->setFont(font);
	textLabel->setWordWrap(true);

	layout->addWidget(titleLabel);
	layout->addWidget(textLabel);
	layout->addSpacing(10);


	titleLabel = new QLabel("When does non-commercial use NOT apply?");
	textLabel = new QLabel(
		"<p>Running Sourcetrail for 'Non-Commercial Use' is not permitted, if your use is in some sense commercial. "
		"Examples of commercial uses are:</p>"
		"<ul><li> you are using Sourcetrail to work on your company's projects (no matter whether you are working on "
		"open-source projects as well),</li>"
		"<li> you are a student and you use Sourcetrail for your work as a freelancer,</li>"
		"<li> you use Sourcetrail in your spare time to help on your friend's new game and you are getting paid for that.</li></ul>"
		"<p>In cases like these you will need a commercial license.</p>"
	);

	titleLabel->setFont(font);
	textLabel->setWordWrap(true);

	layout->addWidget(titleLabel);
	layout->addWidget(textLabel);
	layout->addSpacing(10);


	titleLabel = new QLabel("Not sure?");
	textLabel = new QLabel(
		"Contact mail@sourcetrail.com and explain your intended use in detail, then we may confirm whether a "
		"non-commercial license applies."
	);

	titleLabel->setFont(font);
	textLabel->setWordWrap(true);

	layout->addWidget(titleLabel);
	layout->addWidget(textLabel);
	layout->addStretch();

	QCheckBox* checkbox = new QCheckBox("I confirm solely non-commercial use of Sourcetrail according to above conditions.");
	connect(checkbox, &QCheckBox::toggled,
		[checkbox, this]()
		{
			setNextEnabled(checkbox->isChecked());
		}
	);
	layout->addWidget(checkbox);
	layout->addStretch();

	widget->setLayout(layout);
}

void QtNonCommercialInfoWindow::windowReady()
{
	updateTitle("Non-Commercial Use Confirmation");
	updateCloseButton("Cancel");
	updateNextButton("Confirm");

	setNextEnabled(false);
	setPreviousVisible(false);
}
