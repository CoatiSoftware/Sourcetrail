#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

QtProjectWizzardWindow::QtProjectWizzardWindow(QWidget *parent)
	: QtSettingsWindow(parent)
	, m_content(nullptr)
	, m_previousButton(nullptr)
	, m_showAsPopup(false)
{
}

QtProjectWizzardContent* QtProjectWizzardWindow::content() const
{
	return m_content;
}

void QtProjectWizzardWindow::setContent(QtProjectWizzardContent* content)
{
	m_content = content;
}

void QtProjectWizzardWindow::setup()
{
	setupForm();

	updateTitle("NEW PROJECT");
	updateDoneButton("Next");

	m_previousButton = new QPushButton("Previous");
	m_previousButton->setObjectName("windowButton");
	connect(m_previousButton, SIGNAL(clicked()), this, SLOT(handlePreviousButtonPress()));

	m_buttonsLayout->insertWidget(2, m_previousButton);
	m_buttonsLayout->insertSpacing(3, 3);

	if (m_showAsPopup)
	{
		updateDoneButton("Ok");
		hideCancelButton(true);
		hidePrevious();
		m_title->hide();

		setMaximumSize(QSize(500, 500));
	}

	m_content->windowReady();
}

void QtProjectWizzardWindow::populateWindow(QWidget* widget)
{
	m_content->populateWindow(widget);
}

void QtProjectWizzardWindow::enableNext()
{
	if (m_doneButton)
	{
		m_doneButton->setEnabled(true);
	}
}

void QtProjectWizzardWindow::disableNext()
{
	if (m_doneButton)
	{
		m_doneButton->setEnabled(false);
	}
}

void QtProjectWizzardWindow::hideNext()
{
	if (m_doneButton)
	{
		m_doneButton->hide();
	}
}

void QtProjectWizzardWindow::disablePrevious()
{
	if (m_previousButton)
	{
		m_previousButton->setEnabled(false);
	}
}

void QtProjectWizzardWindow::hidePrevious()
{
	if (m_previousButton)
	{
		m_previousButton->hide();
	}
}

bool QtProjectWizzardWindow::getShowAsPopup() const
{
	return m_showAsPopup;
}

void QtProjectWizzardWindow::setShowAsPopup(bool showAsPopup)
{
	m_showAsPopup = showAsPopup;
}

void QtProjectWizzardWindow::handleCancelButtonPress()
{
	emit canceled();
}

void QtProjectWizzardWindow::handleUpdateButtonPress()
{
	if (m_showAsPopup)
	{
		hide();
		emit closed();
	}

	if (m_content->check())
	{
		m_content->save();
		emit next();
	}
}

void QtProjectWizzardWindow::handlePreviousButtonPress()
{
	m_content->save();

	emit previous();
}
