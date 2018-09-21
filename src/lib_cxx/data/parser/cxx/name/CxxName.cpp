#include "CxxName.h"

#include "utilityString.h"

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

std::wstring CxxName::getTemplateSuffix(const std::vector<std::wstring>& elements) const
{
	if (elements.size())
	{
		return L'<' + utility::join(elements, L", ") + L'>';
	}

	return L"";
}
