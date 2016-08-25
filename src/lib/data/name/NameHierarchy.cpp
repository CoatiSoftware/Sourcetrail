#include "data/name/NameHierarchy.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

std::string NameHierarchy::serialize(NameHierarchy nameHierarchy)
{
	std::string serializedName = "";
	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		if (i > 0)
		{
			serializedName += "\tn";
		}
		serializedName += nameHierarchy[i]->getName() + "\ts";
		serializedName += NameElement::Signature::serialize(nameHierarchy[i]->getSignature());
	}
	return serializedName;
}

NameHierarchy NameHierarchy::deserialize(const std::string& serializedName)
{
	NameHierarchy nameHierarchy;

	std::vector<std::string> serializedNameElements = utility::splitToVector(serializedName, "\tn");
	for (size_t i = 0; i < serializedNameElements.size(); i++)
	{
		std::vector<std::string> nameParts = utility::splitToVector(serializedNameElements[i], "\ts");
		if (nameParts.size() != 2)
		{
			LOG_ERROR("unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
			return NameHierarchy();
		}
		nameHierarchy.push(std::make_shared<NameElement>(nameParts[0], NameElement::Signature::deserialize(nameParts[1])));
	}

	return nameHierarchy;
}

void NameHierarchy::setDelimiter(const std::string& delimiter)
{
	s_delimiter = delimiter;
}

NameHierarchy::NameHierarchy()
{
}

NameHierarchy::NameHierarchy(const std::string& name)
{
	push(std::make_shared<NameElement>(name));
}

NameHierarchy::NameHierarchy(const std::vector<std::string>& names)
{
	for (const std::string& name : names)
	{
		push(std::make_shared<NameElement>(name));
	}
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

std::string NameHierarchy::getQualifiedName() const
{
	std::string name;
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (i > 0)
		{
			name += s_delimiter;
		}
		name += m_elements[i]->getName();
	}
	return name;
}

std::string NameHierarchy::getQualifiedNameWithSignature() const
{
	std::string name = getQualifiedName();
	if (m_elements.size())
	{
		name = m_elements.back()->getSignature().qualifyName(name); // todo: use separator for signature!
	}
	return name;
}

std::string NameHierarchy::getRawName() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getName();
	}
	return "";
}

std::string NameHierarchy::getRawNameWithSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getNameWithSignature();
	}
	return "";
}

std::string NameHierarchy::s_delimiter = "@";
