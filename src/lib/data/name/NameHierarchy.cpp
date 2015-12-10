#include "data/name/NameHierarchy.h"

NameHierarchy::NameHierarchy()
{
}

NameHierarchy::NameHierarchy(const std::string& name)
{
	push(std::make_shared<NameElement>(name));
}

NameHierarchy::~NameHierarchy()
{
}

void NameHierarchy::push(std::shared_ptr<NameElement> element)
{
	m_elements.push_back(element);
}

void NameHierarchy::pop()
{
	m_elements.pop_back();
}

std::shared_ptr<NameElement> NameHierarchy::back() const
{
	return m_elements.back();
}

std::shared_ptr<NameElement> NameHierarchy::operator[](size_t pos) const
{
	return m_elements[pos];
}

size_t NameHierarchy::size() const
{
	return m_elements.size();
}

std::string NameHierarchy::getFullName() const
{
	std::string name;
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		name += m_elements[i]->getFullName();
		if (i + 1 < m_elements.size())
		{
			name += "::";
		}
	}
	return name;
}

std::string NameHierarchy::getName() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getFullName();
	}

	return "";
}
