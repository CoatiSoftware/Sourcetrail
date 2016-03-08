#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"

QtProjectWizzardContentSummary::QtProjectWizzardContentSummary(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
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
		layout->addWidget(advancedLabel, row++, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

		layout->setRowMinimumHeight(row++, 15);
	}

	layout->setRowMinimumHeight(row, 10);
	layout->setRowStretch(row, 1);
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
