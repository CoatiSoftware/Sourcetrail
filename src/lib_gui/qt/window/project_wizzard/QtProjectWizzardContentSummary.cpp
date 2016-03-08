#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"

#include <QCheckBox>

QtProjectWizzardContentSummary::QtProjectWizzardContentSummary(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
	, m_layout(nullptr)
	, m_checkBox(nullptr)
	, m_checkBoxRow(0)
	, m_isForm(false)
{
}

void QtProjectWizzardContentSummary::addContent(QtProjectWizzardContent* content, bool advanced, bool gapBefore)
{
	Element element;
	element.content = content;
	element.advanced = advanced;
	element.gapBefore = gapBefore;
	m_elements.push_back(element);
}

void QtProjectWizzardContentSummary::setIsForm(bool isForm)
{
	m_isForm = isForm;
}

void QtProjectWizzardContentSummary::populateWindow(QGridLayout* layout)
{
	int row = 0;

	if (m_isForm)
	{
		populateForm(layout, row);
		return;
	}

	layout->setRowMinimumHeight(row++, 10);

	for (const Element& element : m_elements)
	{
		if (element.advanced)
		{
			continue;
		}

		if (element.gapBefore)
		{
			layout->setRowMinimumHeight(row++, 20);
		}

		element.content->populateWindow(layout, row);
	}

	layout->setRowMinimumHeight(row, 10);
	layout->setRowStretch(row, 1);
}

void QtProjectWizzardContentSummary::populateForm(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 10);

	bool hasAdvanced = false;
	for (int i = 0; i < 2; i++)
	{
		bool advanced = i > 0;

		for (const Element& element : m_elements)
		{
			if (element.advanced != advanced)
			{
				hasAdvanced = true;
				continue;
			}

			if (element.gapBefore)
			{
				layout->setRowMinimumHeight(row++, 15);
			}

			element.content->populateForm(layout, row);
		}

		if (i > 0 || !hasAdvanced)
		{
			continue;
		}

		QFrame* separator = new QFrame();
		separator->setFrameShape(QFrame::HLine);

		QPalette palette = separator->palette();
		palette.setColor(QPalette::WindowText, Qt::lightGray);
		separator->setPalette(palette);

		layout->addWidget(separator, row++, 0, 1, -1);

		QLabel* advancedLabel = createFormLabel("ADVANCED");
		layout->addWidget(advancedLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

		m_layout = layout;
		m_checkBoxRow = row;
		m_checkBox = new QCheckBox("Show advanced settings");
		layout->addWidget(m_checkBox, row++, QtProjectWizzardWindow::BACK_COL);
		connect(m_checkBox, SIGNAL(clicked(bool)), this, SLOT(advancedToggled(bool)));

		row++;
	}

	layout->setRowMinimumHeight(row, 10);
	layout->setRowStretch(row, 1);

	advancedToggled(false);
}

void QtProjectWizzardContentSummary::load()
{
	for (const Element& element : m_elements)
	{
		element.content->load();
	}
}

void QtProjectWizzardContentSummary::save()
{
	for (const Element& element : m_elements)
	{
		element.content->save();
	}
}

bool QtProjectWizzardContentSummary::check()
{
	for (const Element& element : m_elements)
	{
		if (!element.content->check())
		{
			return false;
		}
	}

	return true;
}

bool QtProjectWizzardContentSummary::isScrollAble() const
{
	return true;
}

void QtProjectWizzardContentSummary::advancedToggled(bool checked)
{
	for (int row = m_checkBoxRow + 1; row < m_layout->rowCount() - 1; row++)
	{
		m_layout->setRowMinimumHeight(row, checked ? 15 : 0);
	}

	for (int i = m_layout->count() - 1; i >= 0; i--)
	{
		int r, c, rs, cs;
		m_layout->getItemPosition(i, &r, &c, &rs, &cs);

		if (r > m_checkBoxRow)
		{
			QLayoutItem *item = m_layout->itemAt(i);
			if (item && item->widget())
			{
				item->widget()->setVisible(checked);
			}
		}
	}
}
