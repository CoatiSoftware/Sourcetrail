#include "CxxName.h"

CxxName::CxxName()
{
}

CxxName::CxxName(std::shared_ptr<CxxName> parent)
	: m_parent(parent)
{
}

void CxxName::setParent(std::shared_ptr<CxxName> parent)
{
	m_parent = parent;
}

std::shared_ptr<CxxName> CxxName::getParent() const
{
	return m_parent;
}

