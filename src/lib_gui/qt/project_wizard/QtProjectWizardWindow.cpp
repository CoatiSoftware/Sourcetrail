#include "QtProjectWizardWindow.h"

#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>

#include "QtProjectWizardContent.h"
#include "utilityQt.h"

QtProjectWizardWindow::QtProjectWizardWindow(QWidget* parent, bool showSeparator)
	: QtWindow(false, parent), m_content(nullptr), m_showSeparator(showSeparator)
{
}

QSize QtProjectWizardWindow::sizeHint() const
{
	if (m_preferredSize.width())
	{
		return m_preferredSize;
	}
	return QSize(750, 620);
}

QtProjectWizardContent* QtProjectWizardWindow::content() const
{
	return m_content;
}

void QtProjectWizardWindow::setContent(QtProjectWizardContent* content)
{
	m_content = content;
}

void QtProjectWizardWindow::setPreferredSize(QSize size)
{
	m_preferredSize = size;
}

void QtProjectWizardWindow::saveContent()
{
	m_content->save();
}

void QtProjectWizardWindow::loadContent()
{
	m_content->load();
}

void QtProjectWizardWindow::refreshContent()
{
	m_content->refresh();
}

void QtProjectWizardWindow::populateWindow(QWidget* widget)
{
	QGridLayout* layout = new QGridLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	layout->setColumnStretch(QtProjectWizardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizardWindow::BACK_COL, 3);

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

void QtProjectWizardWindow::windowReady()
{
	updateTitle(QStringLiteral("NEW SOURCE GROUP"));

	m_content->windowReady();
}

void QtProjectWizardWindow::handleNext()
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

void QtProjectWizardWindow::handlePrevious()
{
	if (m_content)
	{
		m_content->save();
	}

	emit previous();
}
