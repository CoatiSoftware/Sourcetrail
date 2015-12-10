#include "data/name/NameElement.h"

NameElement::NameElement(const std::string& name)
	: m_name(name)
	, m_signature("")
{
}

NameElement::NameElement(const std::string& name, const std::string& signature)
	: m_name(name)
	, m_signature(signature)
{
}

NameElement::~NameElement()
{
}

std::string NameElement::getFullName() const
{
	return m_name;
}

std::string NameElement::getFullSignature() const
{
	return m_signature;
}
