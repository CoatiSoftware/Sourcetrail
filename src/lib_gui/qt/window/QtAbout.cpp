#include "qt/window/QtAbout.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"
#include "utility/utility.h"
#include "utility/Version.h"

QtAbout::QtAbout(QWidget *parent)
	: QtWindow(false, parent)
{
}

QSize QtAbout::sizeHint() const
{
	return QSize(500, 520);
}

void QtAbout::setupAbout()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(FilePath("about/about.css"))).c_str());

	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(10, 10, 10, 0);
	windowLayout->setSpacing(1);
	m_content->setLayout(windowLayout);

	{
		QHBoxLayout* row = new QHBoxLayout();
		windowLayout->addLayout(row);
		{
			QtDeviceScaledPixmap sourcetrailLogo((ResourcePaths::getGuiPath().str() + "about/logo_sourcetrail.png").c_str());
			sourcetrailLogo.scaleToHeight(150);

			QLabel* sourcetrailLogoLabel = new QLabel(this);
			sourcetrailLogoLabel->setPixmap(sourcetrailLogo.pixmap());
			sourcetrailLogoLabel->resize(sourcetrailLogo.width(), sourcetrailLogo.height());
			row->addWidget(sourcetrailLogoLabel);
		}

		row->addSpacing(50);

		{
			QVBoxLayout* column = new QVBoxLayout();
			row->addLayout(column);

			column->addStretch();

			QLabel* developerTitle = new QLabel("<b>Developed by:</b>", this);
			column->addWidget(developerTitle);

			QLabel* developerLabel = new QLabel(
				"Manuel Dobusch\n"
				"Eberhard Gräther\n"
				"Malte Langkabel\n"
				"Viktoria Pfausler\n"
				"Andreas Stallinger\n",
				this
			);
			column->addWidget(developerLabel);
			column->addSpacing(-10);
		}
	}

	windowLayout->addSpacing(10);

	{
		QHBoxLayout* row = new QHBoxLayout();
		windowLayout->addLayout(row);
		QLabel* versionLabel = new QLabel(
			(
				"Version " + Version::getApplicationVersion().toDisplayString() + " - " +
				std::string(utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_32 ? "32" : "64") + " bit"
			).c_str(), 
			this
		);
		row->addWidget(versionLabel);
	}

	windowLayout->addStretch();
	windowLayout->addStretch();

	QLabel* acknowledgementsTitle = new QLabel("<b>Acknowledgements:</b>", this);
	windowLayout->addWidget(acknowledgementsTitle);

	QLabel* acknowledgementsLabel = new QLabel(
		"Sourcetrail (aka Coati) 0.1 was created in the context of education at "
		"<a href=\"http://www.fh-salzburg.ac.at/en/\" style=\"color: white;\">Salzburg University of Applied Sciences</a>. "
		"Coati Software OG takes part in the <a href=\"http://www.startup-salzburg.at/\" style=\"color: white;\">Startup Salzburg</a> initiative. "
		"The development of Sourcetrail was funded by <a href=\"http://awsg.at\" style=\"color: white;\">aws</a>.",
		this
	);
	acknowledgementsLabel->setWordWrap(true);
	acknowledgementsLabel->setOpenExternalLinks(true);
	windowLayout->addWidget(acknowledgementsLabel);

	windowLayout->addStretch();

	{
		QHBoxLayout* row = new QHBoxLayout();
		windowLayout->addLayout(row);

		QLabel* companyLabel = new QLabel(
			"<b>Coati Software OG</b><br />"
			"Jakob-Haringer-Straße 1/127<br />"
			"5020 Salzburg<br />"
			"Austria<br />"
			"<b>support@sourcetrail.com</b><br />"
			"<b><a href=\"https://sourcetrail.com\" style=\"color: white;\">sourcetrail.com</a></b>",
			this);
		companyLabel->setOpenExternalLinks(true);
		row->addWidget(companyLabel);

		{
			QVBoxLayout* column = new QVBoxLayout();
			row->addLayout(column);

			column->addStretch();

			{
				QHBoxLayout* rowAcknowledgementsLogos = new QHBoxLayout();
				column->addLayout(rowAcknowledgementsLogos);

				QtDeviceScaledPixmap fhsLogo((ResourcePaths::getGuiPath().str() + "about/logo_fhs.png").c_str());
				fhsLogo.scaleToHeight(30);
				QLabel* fhsLabel = new QLabel(this);
				fhsLabel->setPixmap(fhsLogo.pixmap());
				fhsLabel->resize(fhsLogo.width(), fhsLogo.height());
				rowAcknowledgementsLogos->addWidget(fhsLabel);

				QtDeviceScaledPixmap awsLogo((ResourcePaths::getGuiPath().str() + "about/logo_aws.png").c_str());
				awsLogo.scaleToHeight(30);
				QLabel* awsLabel = new QLabel(this);
				awsLabel->setPixmap(awsLogo.pixmap());
				awsLabel->resize(awsLogo.width(), awsLogo.height());
				rowAcknowledgementsLogos->addWidget(awsLabel);
			}
		}
	}

	windowLayout->addSpacing(10);
}
