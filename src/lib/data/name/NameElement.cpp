#include "NameElement.h"

#include "logging.h"
#include "utilityString.h"

NameElement::Signature::Signature()
	: m_prefix(L"")
	, m_postfix(L"")
{
}

NameElement::Signature::Signature(std::wstring prefix, std::wstring postfix)
	: m_prefix(std::move(prefix))
	, m_postfix(std::move(postfix))
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

std::wstring NameElement::Signature::getParameterString() const
{
	if (m_postfix.size())
	{
		return utility::substrBeforeLast(m_postfix, L')') + L')';
	}

	return m_postfix;
}

NameElement::NameElement(std::wstring name)
	: m_name(std::move(name))
{
}

NameElement::NameElement(std::wstring name, std::wstring prefix, std::wstring postfix)
	: m_name(std::move(name))
	, m_signature(std::move(prefix), std::move(postfix))
{
}

NameElement::~NameElement()
{
}

const std::wstring& NameElement::getName() const
{
	return m_name;
}

std::wstring NameElement::getNameWithSignature() const
{
	return m_signature.qualifyName(m_name);
}

std::wstring NameElement::getNameWithSignatureParameters() const
{
	return m_name + m_signature.getParameterString();
}

bool NameElement::hasSignature() const
{
	return m_signature.isValid();
}

const NameElement::Signature& NameElement::getSignature()
{
	return m_signature;
}
