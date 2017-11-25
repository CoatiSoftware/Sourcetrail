#include "qt/element/QtHelpButton.h"

#include <QMessageBox>

#include "utility/ResourcePaths.h"

QtHelpButton::QtHelpButton(const QString& helpTitle, const QString& helpText, QWidget* parent)
	: QtIconButton(
		(ResourcePaths::getGuiPath().str() + "window/help.png").c_str(),
		(ResourcePaths::getGuiPath().str() + "window/help_hover.png").c_str(),
		parent)
	, m_helpTitle(helpTitle)
	, m_helpText(helpText)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setMouseTracking(true);

	setToolTip("help");
	setIconSize(QSize(16, 16));

	leaveEvent(nullptr);

	connect(this, &QtHelpButton::clicked, this, &QtHelpButton::handleHelpPress);
}

void QtHelpButton::handleHelpPress()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle("Sourcetrail");
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("<b>" + m_helpTitle + "</b>");
	msgBox.setInformativeText(m_helpText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}
