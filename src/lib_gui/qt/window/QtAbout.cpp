#include "qt/window/QtAbout.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "qt/utility/QtDeviceScaledPixmap.h"
#include "qt/utility/utilityQt.h"
#include "utility/Version.h"
#include "utility/ResourcePaths.h"

QtAbout::QtAbout(QWidget *parent)
	: QtWindow(parent)
{
}

QSize QtAbout::sizeHint() const
{
	return QSize(500, 520);
}

void QtAbout::setupAbout()
{
	m_window->setStyleSheet(
		m_window->styleSheet() +
		"#window { "
			"background: qlineargradient( x1:0 y1:0.4, x2:0 y2:1, stop:0 #2F3F86, stop:1 #1C7BBC );"
			"border: none;"
		"}"
	);

	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "about/about.css").c_str());

	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(30, 30, 30, 20);
	m_window->setLayout(windowLayout);

	windowLayout->addStretch();

	{
		QHBoxLayout* row = new QHBoxLayout();
		windowLayout->addLayout(row);
		{
			QVBoxLayout* column = new QVBoxLayout();
			row->addLayout(column);

			QtDeviceScaledPixmap coatiLogo((ResourcePaths::getGuiPath() + "about/logo_coati.png").c_str());
			coatiLogo.scaleToHeight(180);
			QLabel* coatiLogoLabel = new QLabel(this);
			coatiLogoLabel->setPixmap(coatiLogo.pixmap());
			coatiLogoLabel->resize(coatiLogo.width(), coatiLogo.height());
			column->addWidget(coatiLogoLabel);
		}
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
	{
		QHBoxLayout* row = new QHBoxLayout();
		windowLayout->addLayout(row);
		QLabel* versionLabel = new QLabel(("Version " + Version::getApplicationVersion().toDisplayString()).c_str(), this);
		row->addWidget(versionLabel);
	}

	windowLayout->addStretch();
	windowLayout->addStretch();

	QLabel* acknowledgementsTitle = new QLabel("<b>Acknowledgements:</b>", this);
	windowLayout->addWidget(acknowledgementsTitle);

	QLabel* acknowledgementsLabel = new QLabel(
		"Coati 0.1 was created in the context of education at<br />"
		"<a href=\"http://www.fh-salzburg.ac.at/en/\" style=\"color: white;\">Salzburg University of Applied Sciences</a>.<br />"
		"Coati Software OG takes part in the <a href=\"http://www.startup-salzburg.at/\" style=\"color: white;\">Startup Salzburg</a> initiative.<br />"
		"The development of Coati is funded by <a href=\"http://awsg.at\" style=\"color: white;\">aws</a>.",
		this
	);
	acknowledgementsLabel->setOpenExternalLinks(true);
	windowLayout->addWidget(acknowledgementsLabel);

	windowLayout->addStretch();

	{
		QHBoxLayout* row = new QHBoxLayout();
		windowLayout->addLayout(row);

		QLabel* companyLabel = new QLabel(
			"<b>Coati Software OG</b><br />"
			"<b>Schlossallee 7/1</b><br />"
			"<b>5412 Puch bei Hallein</b><br />"
			"<b>Austria</b><br />"
			"<b>support@coati.io</b><br />"
			"<b><a href=\"https://coati.io\" style=\"color: white;\">coati.io</a></b>",
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

				QtDeviceScaledPixmap fhsLogo((ResourcePaths::getGuiPath() + "about/logo_fhs.png").c_str());
				fhsLogo.scaleToHeight(30);
				QLabel* fhsLabel = new QLabel(this);
				fhsLabel->setPixmap(fhsLogo.pixmap());
				fhsLabel->resize(fhsLogo.width(), fhsLogo.height());
				rowAcknowledgementsLogos->addWidget(fhsLabel);

				QtDeviceScaledPixmap awsLogo((ResourcePaths::getGuiPath() + "about/logo_aws.png").c_str());
				awsLogo.scaleToHeight(30);
				QLabel* awsLabel = new QLabel(this);
				awsLabel->setPixmap(awsLogo.pixmap());
				awsLabel->resize(awsLogo.width(), awsLogo.height());
				rowAcknowledgementsLogos->addWidget(awsLabel);
			}
		}
	}

	windowLayout->addStretch();


	QtDeviceScaledPixmap closePixmap((ResourcePaths::getGuiPath() + "about/icon_close.png").c_str());
	closePixmap.scaleToHeight(20);
	QPushButton* closeButton = new QPushButton(this);
	closeButton->setIcon(QIcon(closePixmap.pixmap()));
	closeButton->setObjectName("closeButton");
	closeButton->move(455, 25);
	closeButton->show();

	connect(closeButton, SIGNAL(clicked()), this, SLOT(handleCloseButtonPress()));
}

void QtAbout::handleCloseButtonPress()
{
	emit canceled();
}
