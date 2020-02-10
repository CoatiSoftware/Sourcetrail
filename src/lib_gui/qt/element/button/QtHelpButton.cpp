#include "QtHelpButton.h"

#include "ResourcePaths.h"

QtHelpButton::QtHelpButton(const QtHelpButtonInfo& info, QWidget* parent)
	: QtIconButton(
		  ResourcePaths::getGuiPath().concatenate(L"window/help.png"),
		  ResourcePaths::getGuiPath().concatenate(L"window/help_hover.png"),
		  parent)
	, m_info(info)
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

void QtHelpButton::setMessageBoxParent(QWidget* messageBoxParent)
{
	m_messageBoxParent = messageBoxParent;
}

void QtHelpButton::handleHelpPress()
{
	m_info.displayMessage(m_messageBoxParent);
}
