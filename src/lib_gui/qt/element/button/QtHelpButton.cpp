#include "QtHelpButton.h"

#include <QMessageBox>

#include "ResourcePaths.h"

QtHelpButton::QtHelpButton(const QString& helpTitle, const QString& helpText, QWidget* parent)
	: QtIconButton(
		  ResourcePaths::getGuiPath().concatenate(L"window/help.png"),
		  ResourcePaths::getGuiPath().concatenate(L"window/help_hover.png"),
		  parent)
	, m_helpTitle(helpTitle)
	, m_helpText(helpText)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect);	  // fixes layouting on Mac
	setMouseTracking(true);

	setToolTip(QStringLiteral("help"));
	setIconSize(QSize(16, 16));
	setObjectName(QStringLiteral("helpButton"));

	leaveEvent(nullptr);

	connect(this, &QtHelpButton::clicked, this, &QtHelpButton::handleHelpPress);
}

void QtHelpButton::handleHelpPress()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(QStringLiteral("Sourcetrail"));
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("<b>" + m_helpTitle + "</b>");
	msgBox.setInformativeText(m_helpText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}
