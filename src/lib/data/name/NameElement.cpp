#include "data/name/NameElement.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

std::wstring NameElement::Signature::serialize(Signature signature)
{
	return signature.m_prefix + L"\tp" + signature.m_postfix;
}

NameElement::Signature NameElement::Signature::deserialize(const std::wstring& serialized)
{
	if (serialized == L"\tp")
	{
		return Signature();
	}

	std::vector<std::wstring> serializedElements = utility::splitToVector(serialized, L"\tp");
	if (serializedElements.size() != 2)
	{
		LOG_ERROR(L"unable to deserialize name signature: " + serialized); // todo: obfuscate serialized!
	}
	return Signature(serializedElements[0], serializedElements[1]);
}

NameElement::Signature::Signature()
	: m_prefix(L"")
	, m_postfix(L"")
{
}

NameElement::Signature::Signature(std::wstring prefix, std::wstring postfix)
	: m_prefix(prefix)
	, m_postfix(postfix)
{
}

std::wstring NameElement::Signature::qualifyName(const std::wstring& name) const
{
	if (!isValid())
	{
		return name;
	}

	std::wstring qualifiedName = m_prefix;
	if (!name.empty())
	{
		if (!m_prefix.empty())
		{
			qualifiedName += L" ";
		}
		qualifiedName += name;
	}
	qualifiedName += m_postfix;

	return qualifiedName;
}

bool NameElement::Signature::isValid() const
{
	return ((m_prefix + m_postfix).size() > 0);
}

const std::wstring& NameElement::Signature::getPrefix() const
{
	return m_prefix;
}

const std::wstring& NameElement::Signature::getPostfix() const
{
	return m_postfix;
}

NameElement::NameElement(const std::wstring& name)
	: m_name(name)
{
}

NameElement::NameElement(const std::wstring& name, const Signature& signature)
	: m_name(name)
	, m_signature(signature)
{
}

NameElement::~NameElement()
{
}

std::wstring NameElement::getName() const
{
	return m_name;
}

std::wstring NameElement::getNameWithSignature() const
{
	return m_signature.qualifyName(m_name);
}

bool NameElement::hasSignature() const
{
	return m_signature.isValid();
}

NameElement::Signature NameElement::getSignature()
{
	return m_signature;
}
