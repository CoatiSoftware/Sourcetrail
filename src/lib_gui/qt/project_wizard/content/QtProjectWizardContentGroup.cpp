#include "QtProjectWizardContentGroup.h"

QtProjectWizardContentGroup::QtProjectWizardContentGroup(QtProjectWizardWindow* window)
	: QtProjectWizardContent(window)
{
}

void QtProjectWizardContentGroup::addContent(QtProjectWizardContent* content)
{
	m_contents.push_back(content);
}

void QtProjectWizardContentGroup::addSpace()
{
	m_contents.push_back(nullptr);
}

bool QtProjectWizardContentGroup::hasContents() const
{
	for (QtProjectWizardContent* content: m_contents)
	{
		if (content)
		{
			return true;
		}
	}

	return false;
}

void QtProjectWizardContentGroup::populate(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 10);

	for (QtProjectWizardContent* content: m_contents)
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

void QtProjectWizardContentGroup::load()
{
	for (QtProjectWizardContent* content: m_contents)
	{
		if (content)
		{
			content->load();
		}
	}
}

void QtProjectWizardContentGroup::save()
{
	for (QtProjectWizardContent* content: m_contents)
	{
		if (content)
		{
			content->save();
		}
	}
}

void QtProjectWizardContentGroup::refresh()
{
	for (QtProjectWizardContent* content: m_contents)
	{
		if (content)
		{
			content->refresh();
		}
	}
}

bool QtProjectWizardContentGroup::check()
{
	for (QtProjectWizardContent* content: m_contents)
	{
		if (content && !content->check())
		{
			return false;
		}
	}

	return true;
}
