#include "data/name/NameElement.h"

NameElement::NameElement(std::string name)
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
