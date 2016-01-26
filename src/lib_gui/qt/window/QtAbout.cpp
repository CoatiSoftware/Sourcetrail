#include "qt/window/QtAbout.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/Version.h"
#include "utility/ResourcePaths.h"

QtAbout::QtAbout(QWidget *parent)
	: QtSettingsWindow(parent)
{
}

QSize QtAbout::sizeHint() const
{
	return QSize(370, 750);
}

void QtAbout::setup()
{
	m_window->setStyleSheet(
		m_window->styleSheet() +
		"#SettingWindow { "
			"background: qlineargradient( x1:0 y1:0.4, x2:0 y2:1, stop:0 #2F3F86, stop:1 #1C7BBC );"
			"border: none;"
		"}"
	);

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "about/about.css").c_str());

	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(30, 30, 30, 20);
	m_window->setLayout(windowLayout);

	QHBoxLayout* rowTitle = new QHBoxLayout();
	windowLayout->addLayout(rowTitle);

	QtDeviceScaledPixmap coatiLogo((ResourcePaths::getGuiPath() + "about/logo.png").c_str());
	coatiLogo.scaleToWidth(120);

	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coatiLogo.pixmap());
	coatiLogoLabel->resize(coatiLogo.width(), coatiLogo.height());
	rowTitle->addWidget(coatiLogoLabel);

	rowTitle->addSpacing(40);

	QVBoxLayout* columnLayout = new QVBoxLayout();
	rowTitle->addLayout(columnLayout);

	columnLayout->addStretch();

	QLabel* versionLabel = new QLabel(("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
	columnLayout->addWidget(versionLabel);

	windowLayout->addStretch();

	QHBoxLayout* rowDeveloperTitle = new QHBoxLayout();
	windowLayout->addLayout(rowDeveloperTitle);

	QLabel* developerTitle = new QLabel("<b>Developed by:</b>", this);
	rowDeveloperTitle->addWidget(developerTitle);

	QHBoxLayout* rowTeam = new QHBoxLayout();
	windowLayout->addLayout(rowTeam);

	QLabel* developerLabel = new QLabel(
		"Manuel Dobusch\n"
		"Eberhard Gräther\n"
		"Malte Langkabel\n"
		"Viktoria Pfausler\n"
		"Andreas Stallinger\n",
		this
	);
	rowTeam->addWidget(developerLabel);

	rowTeam->addSpacing(40);

	QLabel* companyLabel = new QLabel(
		"Coati Software OG\n"
		"Schlossallee 7/1\n"
		"5412 Puch bei Hallein\n"
		"Austria\n"
		"support@coati.io\n",
		this
	);
	rowTeam->addWidget(companyLabel);

	windowLayout->addStretch();

	QHBoxLayout* rowAcknowledgementsTitle = new QHBoxLayout();
	windowLayout->addLayout(rowAcknowledgementsTitle);

	QLabel* acknowledgementsTitle = new QLabel("<b>Acknowledgements:</b>", this);
	rowAcknowledgementsTitle->addWidget(acknowledgementsTitle);


	QLabel* acknowledgementsLabel = new QLabel(
		"Coati 0.1 was created in the context of education \n"
		"at the University of Applied Sciences Salzburg\n"
		"\n"
		"Coati Software OG takes part in the Startup Salzburg\n"
		"initiative.\n"
		"\n"
		"The development of Coati is funded by the AWS\n",
		this
	);
	windowLayout->addWidget(acknowledgementsLabel);

	QHBoxLayout* rowAcknowledgementsLogos = new QHBoxLayout();
	windowLayout->addLayout(rowAcknowledgementsLogos);

	rowAcknowledgementsLogos->addStretch();

	QtDeviceScaledPixmap fhsLogo((ResourcePaths::getGuiPath() + "about/logo_fhs.png").c_str());
	fhsLogo.scaleToHeight(60);

	QLabel* fhsLabel = new QLabel(this);
	fhsLabel->setPixmap(fhsLogo.pixmap());
	fhsLabel->resize(fhsLogo.width(), fhsLogo.height());
	rowAcknowledgementsLogos->addWidget(fhsLabel);

	rowAcknowledgementsLogos->addStretch();

	QtDeviceScaledPixmap awsLogo("data/gui/about/logo_aws.png");
	awsLogo.scaleToHeight(60);
	QLabel* awsLabel = new QLabel(this);
	awsLabel->setPixmap(awsLogo.pixmap());
	awsLabel->resize(awsLogo.width(), awsLogo.height());
	rowAcknowledgementsLogos->addWidget(awsLabel);

	rowAcknowledgementsLogos->addStretch();

	windowLayout->addStretch();

	QPushButton* closeButton = new QPushButton("X", this);
	closeButton->setObjectName("closeButton");
	closeButton->move(320, 20);

	connect(closeButton, SIGNAL(clicked()), this, SLOT(handleUpdateButtonPress()));
}

void QtAbout::handleCancelButtonPress()
{
}

void QtAbout::handleUpdateButtonPress()
{
	emit finished();
}
