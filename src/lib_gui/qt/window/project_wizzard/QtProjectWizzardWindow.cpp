#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>

#include "qt/utility/utilityQt.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "utility/ResourcePaths.h"

QtProjectWizzardWindow::QtProjectWizzardWindow(QWidget *parent)
	: QtWindow(parent)
	, m_content(nullptr)
{
}

QSize QtProjectWizzardWindow::sizeHint() const
{
	if (content())
	{
		return content()->preferredWindowSize();
	}
	return QSize();
}

QtProjectWizzardContent* QtProjectWizzardWindow::content() const
{
	return m_content;
}

void QtProjectWizzardWindow::setContent(QtProjectWizzardContent* content)
{
	m_content = content;
	setScrollAble(content->isScrollAble());
}

void QtProjectWizzardWindow::populateWindow(QWidget* widget)
{
	QGridLayout* layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	int row = 0;
	m_content->populateWindow(layout, row);

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

		if (isScrollAble())
		{
			layout->setColumnMinimumWidth(BACK_COL + 1, 10);
		}

		widget->setLayout(layout);
	}
}

void QtProjectWizzardWindow::windowReady()
{
	updateTitle("NEW PROJECT");

	m_content->windowReady();
}

void QtProjectWizzardWindow::handleNext()
{
	if (isPopup())
	{
		hide();
		emit next();
	}

	m_content->save();

	if (m_content->check())
	{
		emit next();
	}
}

void QtProjectWizzardWindow::handlePrevious()
{
	m_content->save();

	emit previous();
}
