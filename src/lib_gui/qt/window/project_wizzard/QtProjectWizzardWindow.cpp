#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QScrollArea>

#include "qt/utility/utilityQt.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

QtProjectWizzardWindow::QtProjectWizzardWindow(QWidget *parent, bool showSeparator)
	: QtWindow(false, parent)
	, m_content(nullptr)
	, m_showSeparator(showSeparator)
{
}

QSize QtProjectWizzardWindow::sizeHint() const
{
	if (m_preferredSize.width())
	{
		return m_preferredSize;
	}
	return QSize(750, 620);
}

QtProjectWizzardContent* QtProjectWizzardWindow::content() const
{
	return m_content;
}

void QtProjectWizzardWindow::setContent(QtProjectWizzardContent* content)
{
	m_content = content;
}

void QtProjectWizzardWindow::setPreferredSize(QSize size)
{
	m_preferredSize = size;
}

void QtProjectWizzardWindow::saveContent()
{
	m_content->save();
}

void QtProjectWizzardWindow::loadContent()
{
	m_content->load();
}

void QtProjectWizzardWindow::populateWindow(QWidget* widget)
{
	QGridLayout* layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 3);

	layout->setColumnStretch(HELP_COL, 0);
	layout->setColumnStretch(LINE_COL, 0);

	int row = 0;
	m_content->populate(layout, row);

	QFrame* separator = new QFrame();
	separator->setFrameShape(QFrame::VLine);

	QPalette palette = separator->palette();
	palette.setColor(QPalette::WindowText, m_showSeparator ? Qt::lightGray : Qt::transparent);
	separator->setPalette(palette);

	layout->addWidget(separator, 0, LINE_COL, -1, 1);

	if (isScrollAble())
	{
		layout->setColumnMinimumWidth(BACK_COL + 1, 10);
	}

	widget->setLayout(layout);
}

void QtProjectWizzardWindow::windowReady()
{
	updateTitle("NEW SOURCE GROUP");

	m_content->windowReady();
}

void QtProjectWizzardWindow::handleNext()
{
	if (m_content)
	{
		saveContent();

		if (!m_content->check())
		{
			return;
		}
	}

	emit next();
}

void QtProjectWizzardWindow::handlePrevious()
{
	if (m_content)
	{
		m_content->save();
	}

	emit previous();
}
