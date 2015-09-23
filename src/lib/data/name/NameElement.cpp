#include "data/name/NameElement.h"

NameElement::NameElement(const std::string& name)
	: m_name(name)
{
}

NameElement::~NameElement()
{
}

std::string NameElement::getFullName() const
{
	return m_name;
}
