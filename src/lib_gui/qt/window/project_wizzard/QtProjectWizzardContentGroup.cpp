#include "QtProjectWizzardContentGroup.h"

QtProjectWizzardContentGroup::QtProjectWizzardContentGroup(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_isForm(false)
{
}

void QtProjectWizzardContentGroup::addContent(QtProjectWizzardContent* content)
{
	m_contents.push_back(content);

	if (content)
	{
		content->setIsInForm(m_isForm);
	}
}

void QtProjectWizzardContentGroup::addSpace()
{
	m_contents.push_back(nullptr);
}

void QtProjectWizzardContentGroup::setIsForm(bool isForm)
{
	m_isForm = isForm;
}

bool QtProjectWizzardContentGroup::hasContents() const
{
	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			return true;
		}
	}

	return false;
}

void QtProjectWizzardContentGroup::populate(QGridLayout* layout, int& row)
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

void QtProjectWizzardContentGroup::populateForm(QGridLayout* layout, int& row)
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

void QtProjectWizzardContentGroup::load()
{
	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			content->load();
		}
	}
}

void QtProjectWizzardContentGroup::save()
{
	for (QtProjectWizzardContent* content : m_contents)
	{
		if (content)
		{
			content->save();
		}
	}
}

bool QtProjectWizzardContentGroup::check()
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

bool QtProjectWizzardContentGroup::isScrollAble() const
{
	return true;
}
