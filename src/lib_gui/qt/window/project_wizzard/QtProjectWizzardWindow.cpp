#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

QtProjectWizzardWindow::QtProjectWizzardWindow(QWidget *parent)
	: QtSettingsWindow(parent)
	, m_content(nullptr)
	, m_previousButton(nullptr)
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

	updateTitle("NEW PROJECT WIZZARD");
	updateDoneButton("Next");

	m_previousButton = new QPushButton("Previous");
	m_previousButton->setObjectName("windowButton");
	connect(m_previousButton, SIGNAL(clicked()), this, SLOT(handlePreviousButtonPress()));

	m_buttonsLayout->insertWidget(2, m_previousButton);
	m_buttonsLayout->insertSpacing(3, 3);

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

void QtProjectWizzardWindow::handleCancelButtonPress()
{
	emit canceled();
}

void QtProjectWizzardWindow::handleUpdateButtonPress()
{
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
