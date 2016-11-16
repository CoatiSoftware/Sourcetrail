#include "data/parser/cxx/name/CxxQualifierFlags.h"

CxxQualifierFlags::CxxQualifierFlags()
	: m_flags(QUALIFIER_NONE)
{
}

CxxQualifierFlags::CxxQualifierFlags(const char flags)
	: m_flags(flags)
{
}

void CxxQualifierFlags::addQualifier(QualifierType qualifier)
{
	m_flags = m_flags | qualifier;
}

void CxxQualifierFlags::removeQualifier(QualifierType qualifier)
{
	m_flags = m_flags & ~qualifier;
}

bool CxxQualifierFlags::empty() const
{
	return m_flags == QUALIFIER_NONE;
}

std::string CxxQualifierFlags::toString() const
{
	std::string ret = "";
	if (m_flags & QUALIFIER_CONST)
	{
		ret += "const";
	}
	return ret;
}

