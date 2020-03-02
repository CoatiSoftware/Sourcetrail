#include "QtAbout.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "QtDeviceScaledPixmap.h"
#include "ResourcePaths.h"
#include "SqliteIndexStorage.h"
#include "Version.h"
#include "utilityApp.h"
#include "utilityQt.h"

QtAbout::QtAbout(QWidget* parent): QtWindow(false, parent) {}

QSize QtAbout::sizeHint() const
{
	return QSize(450, 480);
}

void QtAbout::setupAbout()
{
	setStyleSheet(
		utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"about/about.css")).c_str());

	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(10, 10, 10, 0);
	windowLayout->setSpacing(1);
	m_content->setLayout(windowLayout);

	{
		QtDeviceScaledPixmap sourcetrailLogo(QString::fromStdWString(
			ResourcePaths::getGuiPath().wstr() + L"about/logo_sourcetrail.png"));
		sourcetrailLogo.scaleToHeight(150);
		QLabel* sourcetrailLogoLabel = new QLabel(this);
		sourcetrailLogoLabel->setPixmap(sourcetrailLogo.pixmap());
		sourcetrailLogoLabel->resize(
			static_cast<int>(sourcetrailLogo.width()), static_cast<int>(sourcetrailLogo.height()));
		windowLayout->addWidget(
			sourcetrailLogoLabel, 0, Qt::Alignment(Qt::AlignmentFlag::AlignHCenter));
	}

	windowLayout->addSpacing(10);

	{
		QLabel* versionLabel = new QLabel(
			("Version " + Version::getApplicationVersion().toDisplayString() + " - " +
			 std::string(
				 utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_32
					 ? "32"
					 : "64") +
			 " bit")
				.c_str(),
			this);
		windowLayout->addWidget(versionLabel, 0, Qt::Alignment(Qt::AlignmentFlag::AlignHCenter));
	}

	{
		QLabel* dbVersionLabel = new QLabel(
			"Database Version " + QString::number(SqliteIndexStorage::getStorageVersion()), this);
		windowLayout->addWidget(dbVersionLabel, 0, Qt::Alignment(Qt::AlignmentFlag::AlignHCenter));
	}

	windowLayout->addStretch();

	{
		QHBoxLayout* layoutHorz1 = new QHBoxLayout();
		windowLayout->addLayout(layoutHorz1);

		QLabel* companyLabel = new QLabel(
			QStringLiteral("<b>Coati Software KG</b><br />"
						   "Jakob-Haringer-Straße 1/127<br />"
						   "5020 Salzburg<br />"
						   "Austria<br />"
						   "<b>support@sourcetrail.com</b><br />"
						   "<b><a href=\"https://sourcetrail.com\" style=\"color: "
						   "white;\">sourcetrail.com</a></b>"));
		companyLabel->setOpenExternalLinks(true);
		layoutHorz1->addWidget(companyLabel);

		layoutHorz1->addSpacing(120);

		QLabel* developerLabel = new QLabel(
			QStringLiteral("<br /><br />"
						   "<b>Team:</b><br />"
						   "Manuel Dobusch<br />"
						   "Eberhard Gräther<br />"
						   "Malte Langkabel<br />"
						   "Viktoria Pfausler<br />"
						   "Andreas Stallinger<br />"));
		developerLabel->setObjectName(QStringLiteral("small"));
		layoutHorz1->addWidget(developerLabel);
	}

	windowLayout->addStretch();

	QLabel* acknowledgementsLabel = new QLabel(QStringLiteral(
		"<b>Acknowledgements:</b><br />"
		"Sourcetrail (aka Coati) 0.1 was created in the context of education at "
		"<a href=\"http://www.fh-salzburg.ac.at/en/\" style=\"color: white;\">Salzburg University "
		"of Applied Sciences</a>.<br />"
		"Coati Software KG is member of <a href=\"http://www.startup-salzburg.at/\" style=\"color: "
		"white;\">Startup Salzburg</a>.<br />"
		"The development of Sourcetrail was funded by <a href=\"http://awsg.at\" style=\"color: "
		"white;\">aws</a>."));
	acknowledgementsLabel->setObjectName(QStringLiteral("small"));
	acknowledgementsLabel->setWordWrap(true);
	acknowledgementsLabel->setOpenExternalLinks(true);
	windowLayout->addWidget(acknowledgementsLabel);

	windowLayout->addSpacing(10);
}
