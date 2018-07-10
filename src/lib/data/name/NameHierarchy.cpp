#include "data/name/NameHierarchy.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

std::wstring NameHierarchy::serialize(const NameHierarchy& nameHierarchy)
{
	std::wstring serializedName = nameDelimiterTypeToString(nameHierarchy.getDelimiter()) + L"\tm";
	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		if (i > 0)
		{
			serializedName += L"\tn";
		}
		serializedName += nameHierarchy[i]->getName() + L"\ts";
		serializedName += NameElement::Signature::serialize(nameHierarchy[i]->getSignature());
	}
	return serializedName;
}

NameHierarchy NameHierarchy::deserialize(const std::wstring& serializedName)
{
	std::vector<std::wstring> serializedNameAndMetaElements = utility::splitToVector(serializedName, L"\tm");
	if (serializedNameAndMetaElements.size() != 2)
	{
		LOG_ERROR(L"unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
		return NameHierarchy(NAME_DELIMITER_UNKNOWN);
	}

	const NameDelimiterType delimiter = stringToNameDelimiterType(serializedNameAndMetaElements[0]);
	NameHierarchy nameHierarchy(delimiter);

	std::vector<std::wstring> serializedNameElements = utility::splitToVector(serializedNameAndMetaElements[1], L"\tn");
	for (size_t i = 0; i < serializedNameElements.size(); i++)
	{
		std::vector<std::wstring> nameParts = utility::splitToVector(serializedNameElements[i], L"\ts");
		if (nameParts.size() != 2)
		{
			LOG_ERROR(L"unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
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

NameHierarchy::NameHierarchy(const std::wstring& name, const NameDelimiterType delimiter)
	: m_delimiter(delimiter)
{
	push(std::make_shared<NameElement>(name));
}

NameHierarchy::NameHierarchy(const std::vector<std::wstring>& names, const NameDelimiterType delimiter)
	: m_delimiter(delimiter)
{
	for (const std::wstring& name : names)
	{
		push(std::make_shared<NameElement>(name));
	}
}

NameHierarchy::NameHierarchy(const NameHierarchy& other)
	: m_elements(other.m_elements)
	, m_delimiter(other.m_delimiter)
{
}

NameHierarchy::NameHierarchy(NameHierarchy&& other)
	: m_elements(std::move(other.m_elements))
	, m_delimiter(other.m_delimiter)
{
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

NameHierarchy& NameHierarchy::operator=(const NameHierarchy& other)
{
	m_elements = other.m_elements;
	m_delimiter = other.m_delimiter;
	return *this;
}

NameHierarchy& NameHierarchy::operator=(NameHierarchy&& other)
{
	m_elements = std::move(other.m_elements);
	m_delimiter = other.m_delimiter;
	return *this;
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

std::wstring NameHierarchy::getQualifiedName() const
{
	std::wstring name;
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

std::wstring NameHierarchy::getQualifiedNameWithSignature() const
{
	std::wstring name = getQualifiedName();
	if (m_elements.size())
	{
		name = m_elements.back()->getSignature().qualifyName(name); // todo: use separator for signature!
	}
	return name;
}

std::wstring NameHierarchy::getRawName() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getName();
	}
	return L"";
}

std::wstring NameHierarchy::getRawNameWithSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getNameWithSignature();
	}
	return L"";
}

std::wstring NameHierarchy::getRawNameWithSignatureParameters() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getNameWithSignatureParameters();
	}
	return L"";
}

bool NameHierarchy::hasSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back()->hasSignature();
	}

	return false;
}

NameElement::Signature NameHierarchy::getSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back()->getSignature(); // todo: use separator for signature!
	}

	return NameElement::Signature();
}
