#include "QtScrollSpeedChangeListener.h"

#include <cmath>

#include <QScrollBar>

#include "ApplicationSettings.h"

QtScrollSpeedChangeListener::QtScrollSpeedChangeListener()
	: m_changeScrollSpeedFunctor(
		  std::bind(&QtScrollSpeedChangeListener::doChangeScrollSpeed, this, std::placeholders::_1))
	, m_scrollBar(nullptr)
	, m_singleStep(1)
{
}

void QtScrollSpeedChangeListener::setScrollBar(QScrollBar* scrollbar)
{
	m_scrollBar = scrollbar;
	m_singleStep = scrollbar->singleStep();

	doChangeScrollSpeed(ApplicationSettings::getInstance()->getScrollSpeed());
}

void QtScrollSpeedChangeListener::handleMessage(MessageScrollSpeedChange* message)
{
	m_changeScrollSpeedFunctor(message->scrollSpeed);
}

void QtScrollSpeedChangeListener::doChangeScrollSpeed(float scrollSpeed)
{
	if (m_scrollBar)
	{
		m_scrollBar->setSingleStep(static_cast<int>(std::ceil(m_singleStep * scrollSpeed)));
	}
}
