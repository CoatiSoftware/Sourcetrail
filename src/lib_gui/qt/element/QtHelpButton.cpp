#include "qt/element/QtHelpButton.h"

#include <QMessageBox>

#include "utility/ResourcePaths.h"

QtHelpButton::QtHelpButton(const QString& helpText, QWidget* parent)
	: QtIconButton(
		(ResourcePaths::getGuiPath() + "window/help.png").c_str(),
		(ResourcePaths::getGuiPath() + "window/help_hover.png").c_str(),
		parent)
	, m_helpText(helpText)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setMouseTracking(true);

	setToolTip("help");

	leaveEvent(nullptr);

	connect(this, SIGNAL(clicked()), this, SLOT(handleHelpPress()));
}

void QtHelpButton::handleHelpPress()
{
	QMessageBox msgBox;
	msgBox.setText("Help");
	msgBox.setInformativeText(m_helpText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}
