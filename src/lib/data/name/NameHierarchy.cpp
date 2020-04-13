#include "NameHierarchy.h"

#include <sstream>

#include "../../utility/logging/logging.h"
#include "../../../lib_utility/utility/utilityString.h"

namespace
{
const std::wstring META_DELIMITER = L"\tm";
const std::wstring NAME_DELIMITER = L"\tn";
const std::wstring PART_DELIMITER = L"\ts";
const std::wstring SIGNATURE_DELIMITER = L"\tp";
}	 // namespace

std::wstring NameHierarchy::serialize(const NameHierarchy& nameHierarchy)
{
	return serializeRange(nameHierarchy, 0, nameHierarchy.size());
}

std::wstring NameHierarchy::serializeRange(const NameHierarchy& nameHierarchy, size_t first, size_t last)
{
	std::wstringstream ss;
	ss << nameHierarchy.getDelimiter();
	ss << META_DELIMITER;
	for (size_t i = first; i < last && i < nameHierarchy.size(); i++)
	{
		if (i > 0)
		{
			ss << NAME_DELIMITER;
		}

		ss << nameHierarchy[i].getName() << PART_DELIMITER;
		ss << nameHierarchy[i].getSignature().getPrefix();
		ss << SIGNATURE_DELIMITER;
		ss << nameHierarchy[i].getSignature().getPostfix();
	}
	return ss.str();
}

NameHierarchy NameHierarchy::deserialize(const std::wstring& serializedName)
{
	size_t mpos = serializedName.find(META_DELIMITER);
	if (mpos == std::wstring::npos)
	{
		LOG_ERROR(L"unable to deserialize name hierarchy: " + serializedName);	  // todo: obfuscate
																				  // serializedName!
		return NameHierarchy(NAME_DELIMITER_UNKNOWN);
	}

	NameHierarchy nameHierarchy(serializedName.substr(0, mpos));

	size_t npos = mpos + META_DELIMITER.size();
	while (npos != std::wstring::npos && npos < serializedName.size())
	{
		// name
		size_t spos = serializedName.find(PART_DELIMITER, npos);
		if (spos == std::wstring::npos)
		{
			LOG_ERROR(
				L"unable to deserialize name hierarchy: " +
				serializedName);	// todo: obfuscate serializedName!
			return NameHierarchy(NAME_DELIMITER_UNKNOWN);
		}

		std::wstring name = serializedName.substr(npos, spos - npos);
		spos += PART_DELIMITER.size();

		// signature
		size_t ppos = serializedName.find(SIGNATURE_DELIMITER, spos);
		if (ppos == std::wstring::npos)
		{
			LOG_ERROR(
				L"unable to deserialize name hierarchy: " +
				serializedName);	// todo: obfuscate serializedName!
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

		nameHierarchy.push(NameElement(std::move(name), std::move(prefix), std::move(postfix)));
	}

	// TODO: replace duplicate main definition fix with better solution
	if (nameHierarchy.size() == 1 && nameHierarchy.back().hasSignature() &&
		!nameHierarchy.back().getName().empty() && nameHierarchy.back().getName()[0] == '.' &&
		utility::isPrefix<std::wstring>(L".:main:.", nameHierarchy.back().getName()))
	{
		NameElement::Signature sig = nameHierarchy.back().getSignature();
		nameHierarchy.pop();
		nameHierarchy.push(NameElement(L"main", sig.getPrefix(), sig.getPostfix()));
	}

	return nameHierarchy;
}

const std::wstring& NameHierarchy::getDelimiter() const
{
	return m_delimiter;
}

void NameHierarchy::setDelimiter(std::wstring delimiter)
{
	m_delimiter = std::move(delimiter);
}

NameHierarchy::NameHierarchy(std::wstring delimiter): m_delimiter(std::move(delimiter)) {}

NameHierarchy::NameHierarchy(const std::vector<std::wstring>& names, std::wstring delimiter)
	: m_delimiter(std::move(delimiter))
{
	for (const std::wstring& name: names)
	{
		push(name);
	}
}

NameHierarchy::NameHierarchy(std::wstring name, std::wstring delimiter)
	: m_delimiter(std::move(delimiter))
{
	push(std::move(name));
}

NameHierarchy::NameHierarchy(const NameDelimiterType delimiterType)
	: NameHierarchy(nameDelimiterTypeToString(delimiterType))
{
}

NameHierarchy::NameHierarchy(std::wstring name, const NameDelimiterType delimiterType)
	: NameHierarchy(name, nameDelimiterTypeToString(delimiterType))
{
}

NameHierarchy::NameHierarchy(const std::vector<std::wstring>& names, const NameDelimiterType delimiterType)
	: NameHierarchy(names, nameDelimiterTypeToString(delimiterType))
{
}

NameHierarchy::NameHierarchy(const NameHierarchy& other)
	: m_elements(other.m_elements), m_delimiter(other.m_delimiter)
{
}

NameHierarchy::NameHierarchy(NameHierarchy&& other)
	: m_elements(std::move(other.m_elements)), m_delimiter(std::move(other.m_delimiter))
{
}

NameHierarchy::~NameHierarchy() {}

void NameHierarchy::push(NameElement element)
{
	m_elements.emplace_back(std::move(element));
}

void NameHierarchy::push(std::wstring name)
{
	m_elements.emplace_back(std::move(name));
}

void NameHierarchy::pop()
{
	m_elements.pop_back();
}

NameElement& NameHierarchy::back()
{
	return m_elements.back();
}

const NameElement& NameHierarchy::back() const
{
	return m_elements.back();
}

NameElement& NameHierarchy::operator[](size_t pos)
{
	return m_elements[pos];
}

const NameElement& NameHierarchy::operator[](size_t pos) const
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
	m_delimiter = std::move(other.m_delimiter);
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
			ss << m_delimiter;
		}
		ss << m_elements[i].getName();
	}
	return ss.str();
}

std::wstring NameHierarchy::getQualifiedNameWithSignature() const
{
	std::wstring name = getQualifiedName();
	if (m_elements.size())
	{
		name = m_elements.back().getSignature().qualifyName(
			name);	  // todo: use separator for signature!
	}
	return name;
}

std::wstring NameHierarchy::getRawName() const
{
	if (m_elements.size())
	{
		return m_elements.back().getName();
	}
	return L"";
}

std::wstring NameHierarchy::getRawNameWithSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back().getNameWithSignature();
	}
	return L"";
}

std::wstring NameHierarchy::getRawNameWithSignatureParameters() const
{
	if (m_elements.size())
	{
		return m_elements.back().getNameWithSignatureParameters();
	}
	return L"";
}

bool NameHierarchy::hasSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back().hasSignature();
	}

	return false;
}

NameElement::Signature NameHierarchy::getSignature() const
{
	if (m_elements.size())
	{
		return m_elements.back().getSignature();	// todo: use separator for signature!
	}

	return NameElement::Signature();
}
