#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"

QtProjectWizzardContentSummary::QtProjectWizzardContentSummary(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_isForm(false)
{
}

void QtProjectWizzardContentSummary::addContent(QtProjectWizzardContent* content)
{
	m_contents.push_back(content);

	if (content)
	{
		content->setIsInForm(m_isForm);
	}
}

void QtProjectWizzardContentSummary::addSpace()
{
	m_contents.push_back(nullptr);
}

void QtProjectWizzardContentSummary::setIsForm(bool isForm)
{
	m_isForm = isForm;
}

void QtProjectWizzardContentSummary::populate(QGridLayout* layout, int& row)
{
	if (m_isForm)
	{
		populateForm(layout, row);
		return;
	}

	layout->setRowMinimumHeight(row++, 10);

	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			content->populate(layout, row);
		}
		else
		{
			layout->setRowMinimumHeight(row++, 20);
		}
	}

	layout->setRowMinimumHeight(row, 10);
	layout->setRowStretch(row, 1);
}

void QtProjectWizzardContentSummary::populateForm(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 10);

	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			content->populate(layout, row);
		}
		else
		{
			layout->setRowMinimumHeight(row++, 15);
		}
	}

	layout->setRowMinimumHeight(row, 10);
	layout->setRowStretch(row, 1);
}

void QtProjectWizzardContentSummary::load()
{
	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			content->load();
		}
	}
}

void QtProjectWizzardContentSummary::save()
{
	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			content->save();
		}
	}
}

bool QtProjectWizzardContentSummary::check()
{
	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content && !content->check())
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
