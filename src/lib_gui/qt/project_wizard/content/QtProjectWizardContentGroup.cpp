#include "QtProjectWizardContentGroup.h"

QtProjectWizardContentGroup::QtProjectWizardContentGroup(QtProjectWizardWindow* window)
	: QtProjectWizardContent(window), m_isForm(false)
{
}

void QtProjectWizardContentGroup::addContent(QtProjectWizardContent* content)
{
	m_contents.push_back(content);

	if (content)
	{
		content->setIsInForm(m_isForm);
	}
}

void QtProjectWizardContentGroup::addSpace()
{
	m_contents.push_back(nullptr);
}

void QtProjectWizardContentGroup::setIsForm(bool isForm)
{
	m_isForm = isForm;
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
	if (m_isForm)
	{
		populateForm(layout, row);
		return;
	}

	layout->setRowMinimumHeight(row++, 10);

	for (QtProjectWizardContent* content: m_contents)
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

void QtProjectWizardContentGroup::populateForm(QGridLayout* layout, int& row)
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

bool QtProjectWizardContentGroup::isScrollAble() const
{
	return true;
}
