#include "data/name/NameHierarchy.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

std::string NameHierarchy::serialize(NameHierarchy nameHierarchy)
{
	std::string serializedName = "";
	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		serializedName += nameHierarchy[i]->getFullName() + "\t";
		serializedName += nameHierarchy[i]->getFullSignature();
		if (i + 1 < nameHierarchy.size())
			serializedName += "\n";
	}
	return serializedName;
}

NameHierarchy NameHierarchy::deserialize(const std::string& serializedName)
{
	NameHierarchy nameHierarchy;
	
	std::vector<std::string> serializedNameElements = utility::splitToVector(serializedName, "\n");
	for (size_t i = 0; i < serializedNameElements.size(); i++)
	{
		std::vector<std::string> nameParts = utility::splitToVector(serializedNameElements[i], "\t");
		if (nameParts.size() != 2)
		{
			LOG_ERROR("unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
			return NameHierarchy();
		}
		nameHierarchy.push(std::make_shared<NameElement>(nameParts[0], nameParts[1]));
	}

	return nameHierarchy;
}

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
	if (m_elements.size() > 0)
	{
		return m_elements.back();
	}
	return std::shared_ptr<NameElement>();
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
