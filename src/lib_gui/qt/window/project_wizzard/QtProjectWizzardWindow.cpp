#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>

#include "qt/utility/utilityQt.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "utility/ResourcePaths.h"

QtProjectWizzardWindow::QtProjectWizzardWindow(QWidget *parent)
	: QtSettingsWindow(parent)
	, m_content(nullptr)
	, m_previousButton(nullptr)
	, m_showAsPopup(false)
	, m_scrollAble(false)
{
}

QtProjectWizzardContent* QtProjectWizzardWindow::content() const
{
	return m_content;
}

void QtProjectWizzardWindow::setContent(QtProjectWizzardContent* content)
{
	m_content = content;
	m_scrollAble = content->isScrollAble();
}

void QtProjectWizzardWindow::setup()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "project_wizzard/window.css").c_str());
	QVBoxLayout* windowLayout = new QVBoxLayout();
	windowLayout->setContentsMargins(25, 30, 25, 20);

	m_title = new QLabel();
	m_title->setObjectName("title");
	windowLayout->addWidget(m_title);
	windowLayout->addSpacing(10);

	QWidget* contentWidget = new QWidget();
	contentWidget->setObjectName("form");

	populateWindow(contentWidget);

	if (m_scrollAble)
	{
		QScrollArea* scrollArea = new QScrollArea();
		scrollArea->setObjectName("formArea");
		scrollArea->setFrameShadow(QFrame::Plain);
		scrollArea->setWidgetResizable(true);

		scrollArea->setWidget(contentWidget);
		windowLayout->addWidget(scrollArea);
	}
	else
	{
		windowLayout->addWidget(contentWidget);
		windowLayout->setStretchFactor(contentWidget, 1);
	}

	showButtons(windowLayout);

	m_window->setLayout(windowLayout);

	resize(content()->preferredWindowSize());


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
	}

	m_content->windowReady();
}

void QtProjectWizzardWindow::populateWindow(QWidget* widget)
{
	QGridLayout* layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	m_content->populateWindow(layout);

	if (layout->columnCount() < 2)
	{
		m_content->populateWindow(widget);
	}
	else
	{
		QFrame* separator = new QFrame();
		separator->setFrameShape(QFrame::VLine);

		QPalette palette = separator->palette();
		palette.setColor(QPalette::WindowText, Qt::lightGray);
		separator->setPalette(palette);

		layout->addWidget(separator, 0, LINE_COL, -1, 1);

		layout->setColumnStretch(HELP_COL, 0);
		layout->setColumnStretch(LINE_COL, 0);

		if (m_scrollAble)
		{
			layout->setColumnMinimumWidth(BACK_COL + 1, 10);
		}

		widget->setLayout(layout);
	}
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

void QtProjectWizzardWindow::setShowAsPopup(bool showAsPopup)
{
	m_showAsPopup = showAsPopup;
}

void QtProjectWizzardWindow::setScrollAble(bool scrollAble)
{
	m_scrollAble = scrollAble;
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
