#include "data/name/NameElement.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

std::string NameElement::Signature::serialize(Signature signature)
{
	return signature.m_prefix + "\tp" + signature.m_postfix;
}

NameElement::Signature NameElement::Signature::deserialize(const std::string& serialized)
{
	if (serialized == "\tp")
	{
		return Signature();
	}

	std::vector<std::string> serializedElements = utility::splitToVector(serialized, "\tp");
	if (serializedElements.size() != 2)
	{
		LOG_ERROR("unable to deserialize name signature: " + serialized); // todo: obfuscate serialized!
	}
	return Signature(serializedElements[0], serializedElements[1]);
}

NameElement::Signature::Signature()
	: m_prefix("")
	, m_postfix("")
{
}

NameElement::Signature::Signature(std::string prefix, std::string postfix)
	: m_prefix(prefix)
	, m_postfix(postfix)
{
}

std::string NameElement::Signature::qualifyName(const std::string& name) const
{
	if (!isValid())
	{
		return name;
	}

	std::string qualifiedName = m_prefix;
	if (!name.empty())
	{
		if (!m_prefix.empty())
		{
			qualifiedName += " ";
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

NameElement::NameElement(const std::string& name)
	: m_name(name)
{
}

NameElement::NameElement(const std::string& name, const Signature& signature)
	: m_name(name)
	, m_signature(signature)
{
}

NameElement::~NameElement()
{
}

std::string NameElement::getName() const
{
	return m_name;
}

std::string NameElement::getNameWithSignature() const
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
