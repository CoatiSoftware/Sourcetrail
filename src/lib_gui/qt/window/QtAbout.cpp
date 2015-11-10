#include "qt/window/QtAbout.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/Version.h"

QtAbout::QtAbout(QWidget *parent)
	: QtSettingsWindow(parent)
{
	raise();
}

QSize QtAbout::sizeHint() const
{
	return QSize(370, 550);
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

	setStyleSheet(utility::getStyleSheet("data/gui/about/about.css").c_str());

	QtDeviceScaledPixmap coatiLogo("data/gui/about/logo.png");
	coatiLogo.scaleToWidth(180);
	QLabel* coatiLogoLabel = new QLabel(this);
	coatiLogoLabel->setPixmap(coatiLogo.pixmap());
	coatiLogoLabel->resize(coatiLogo.width(), coatiLogo.height());
	coatiLogoLabel->move(8, 20);

	QLabel* versionLabel = new QLabel(("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
	versionLabel->move(210, 181);

	QLabel* developerTitle = new QLabel("Developed by:", this);
	developerTitle->move(30, 220);

	QLabel* companyLabel = new QLabel(
		"Coati Software OG\n"
		"Schlossallee 7/1\n"
		"5412 Puch bei Hallein\n"
		"Austria\n"
		"support@coati.io\n",
		this
	);
	companyLabel->move(210, 250);

	QLabel* developerLabel = new QLabel(
		"Manuel Dobusch\n"
		"Eberhard Gräther\n"
		"Malte Langkabel\n"
		"Viktoria Pfausler\n"
		"Andreas Stallinger\n",
		this
	);
	developerLabel->move(30, 250);

	QLabel* acknowledgementsTitle = new QLabel("Acknowledgements:", this);
	acknowledgementsTitle->setObjectName("small");
	acknowledgementsTitle->move(30, 355);

	QLabel* acknowledgementsLabel = new QLabel(
		"Coati 0.1 was created in the context of education at",
		this
	);
	acknowledgementsLabel->setObjectName("small");
	acknowledgementsLabel->move(30, 385);

	QtDeviceScaledPixmap fhsLogo("data/gui/about/logo_fhs.png");
	fhsLogo.scaleToWidth(150);
	QLabel* fhsLabel = new QLabel(this);
	fhsLabel->setPixmap(fhsLogo.pixmap());
	fhsLabel->resize(fhsLogo.width(), fhsLogo.height());
	fhsLabel->move(115, 410);

	QLabel* acknowledgementsLabel2 = new QLabel(
		"Coati Software OG takes part in the Startup Salzburg\ninitiative.",
		this
	);
	acknowledgementsLabel2->setObjectName("small");
	acknowledgementsLabel2->move(30, 465);

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
