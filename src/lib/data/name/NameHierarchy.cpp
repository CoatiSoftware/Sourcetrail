#include "data/name/NameHierarchy.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

std::string NameHierarchy::serialize(NameHierarchy nameHierarchy)
{
	std::string serializedName = nameDelimiterTypeToString(nameHierarchy.getDelimiter()) + "\tm";
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
	std::vector<std::string> serializedNameAndMetaElements = utility::splitToVector(serializedName, "\tm");
	if (serializedNameAndMetaElements.size() != 2)
	{
		LOG_ERROR("unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
		return NameHierarchy(NAME_DELIMITER_UNKNOWN);
	}

	const NameDelimiterType delimiter = stringToNameDelimiterType(serializedNameAndMetaElements[0]);
	NameHierarchy nameHierarchy(delimiter);

	std::vector<std::string> serializedNameElements = utility::splitToVector(serializedNameAndMetaElements[1], "\tn");
	for (size_t i = 0; i < serializedNameElements.size(); i++)
	{
		std::vector<std::string> nameParts = utility::splitToVector(serializedNameElements[i], "\ts");
		if (nameParts.size() != 2)
		{
			LOG_ERROR("unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
			return NameHierarchy(delimiter);
		}
		nameHierarchy.push(std::make_shared<NameElement>(nameParts[0], NameElement::Signature::deserialize(nameParts[1])));
	}

	return nameHierarchy;
}

NameDelimiterType NameHierarchy::getDelimiter() const
{
	return m_delimiter;
}

void NameHierarchy::setDelimiter(const NameDelimiterType delimiter)
{
	m_delimiter = delimiter;
}

NameHierarchy::NameHierarchy(const NameDelimiterType delimiter)
	: m_delimiter(delimiter)
{
}

NameHierarchy::NameHierarchy(const std::string& name, const NameDelimiterType delimiter)
	: m_delimiter(delimiter)
{
	push(std::make_shared<NameElement>(name));
}

NameHierarchy::NameHierarchy(const std::vector<std::string>& names, const NameDelimiterType delimiter)
	: m_delimiter(delimiter)
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

NameHierarchy NameHierarchy::getRange(size_t first, size_t last) const
{
	NameHierarchy hierarchy(m_delimiter);

	for (size_t i = first; i < last; i++)
	{
		hierarchy.push(m_elements[i]);
	}

	return hierarchy;
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
			name += nameDelimiterTypeToString(m_delimiter);
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
