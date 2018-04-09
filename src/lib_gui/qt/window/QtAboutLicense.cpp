#include "qt/window/QtAboutLicense.h"

#include <QVBoxLayout>
#include <QLabel>

#include "licenses.h"

QtAboutLicense::QtAboutLicense(QWidget *parent)
	: QtWindow(false, parent)
{
	setScrollAble(true);
}

QSize QtAboutLicense::sizeHint() const
{
	return QSize(600, 600);
}

void QtAboutLicense::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);

	for (ThirdPartyLicense license : licenses3rdParties)
	{
		QLabel* licenseName = new QLabel();
		licenseName->setText(
			QString::fromLatin1(license.name) + 
			QString::fromLatin1(
				std::string(license.version).empty() ? 
				"" : 
				(std::string(" (v") + license.version + ")").c_str()
			)
		);
		QFont _font = licenseName->font();
		_font.setPixelSize(36);
		_font.setBold(true);
		licenseName->setFont(_font);
		layout->addWidget(licenseName);

		QLabel* licenseURL = new QLabel();
		licenseURL->setText(QString::fromLatin1("<a href=\"%1\">%1</a>")
									.arg(QString::fromLatin1(license.url)));
		licenseURL->setOpenExternalLinks(true);
		layout->addWidget(licenseURL);

		QLabel* licenseText = new QLabel();
		licenseText->setFixedWidth(450);
		licenseText->setWordWrap(true);
		licenseText->setText(QString::fromLatin1(license.license));
		layout->addWidget(licenseText);

		layout->addSpacing(50);
	}

	widget->setLayout(layout);
}

void QtAboutLicense::windowReady()
{
	updateTitle("3rd Party Licenses");
	updateCloseButton("Close");

	setNextVisible(false);
	setPreviousVisible(false);
}
