#include "NameHierarchy.h"

#include <sstream>

#include "logging.h"
#include "utilityString.h"

namespace
{
	const std::wstring META_DELIMITER = L"\tm";
	const std::wstring NAME_DELIMITER = L"\tn";
	const std::wstring PART_DELIMITER = L"\ts";
	const std::wstring SIGNATURE_DELIMITER = L"\tp";
}

std::wstring NameHierarchy::serialize(const NameHierarchy& nameHierarchy)
{
	return serializeRange(nameHierarchy, 0, nameHierarchy.size());
}

std::wstring NameHierarchy::serializeRange(const NameHierarchy& nameHierarchy, size_t first, size_t last)
{
	std::wstringstream ss;
	ss << nameDelimiterTypeToString(nameHierarchy.getDelimiter());
	ss << META_DELIMITER;
	for (size_t i = first; i < last && i < nameHierarchy.size(); i++)
	{
		if (i > 0)
		{
			ss << NAME_DELIMITER;
		}

		ss << nameHierarchy[i]->getName() << PART_DELIMITER;
		ss << nameHierarchy[i]->getSignature().getPrefix();
		ss << SIGNATURE_DELIMITER;
		ss << nameHierarchy[i]->getSignature().getPostfix();
	}
	return ss.str();
}

NameHierarchy NameHierarchy::deserialize(const std::wstring& serializedName)
{
	size_t mpos = serializedName.find(META_DELIMITER);
	if (mpos == std::wstring::npos)
	{
		LOG_ERROR(L"unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
		return NameHierarchy(NAME_DELIMITER_UNKNOWN);
	}

	NameHierarchy nameHierarchy(stringToNameDelimiterType(serializedName.substr(0, mpos)));

	size_t npos = mpos + META_DELIMITER.size();
	while (npos != std::wstring::npos && npos < serializedName.size())
	{
		// name
		size_t spos = serializedName.find(PART_DELIMITER, npos);
		if (spos == std::wstring::npos)
		{
			LOG_ERROR(L"unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
			return NameHierarchy(NAME_DELIMITER_UNKNOWN);
		}

		std::wstring name = serializedName.substr(npos, spos - npos);
		spos += PART_DELIMITER.size();

		// signature
		size_t ppos = serializedName.find(SIGNATURE_DELIMITER, spos);
		if (ppos == std::wstring::npos)
		{
			LOG_ERROR(L"unable to deserialize name hierarchy: " + serializedName); // todo: obfuscate serializedName!
			return NameHierarchy(NAME_DELIMITER_UNKNOWN);
		}

		std::wstring prefix = serializedName.substr(spos, ppos - spos);
		ppos += SIGNATURE_DELIMITER.size();

		std::wstring postfix;
		npos = serializedName.find(NAME_DELIMITER, ppos);
		if (npos == std::wstring::npos)
		{
			postfix = serializedName.substr(ppos, std::wstring::npos);
		}
		else
		{
			postfix = serializedName.substr(ppos, npos - ppos);
			npos += NAME_DELIMITER.size();
		}

		nameHierarchy.push(std::make_shared<NameElement>(std::move(name), std::move(prefix), std::move(postfix)));
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

NameHierarchy::NameHierarchy(std::wstring name, const NameDelimiterType delimiter)
	: m_delimiter(delimiter)
{
	push(std::make_shared<NameElement>(std::move(name)));
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

void NameHierarchy::push(std::wstring name)
{
	m_elements.push_back(std::make_shared<NameElement>(std::move(name)));
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
	std::wstringstream ss;
	for (size_t i = 0; i < m_elements.size(); i++)
	{
		if (i > 0)
		{
			ss << nameDelimiterTypeToString(m_delimiter);
		}
		ss << m_elements[i]->getName();
	}
	return ss.str();
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
