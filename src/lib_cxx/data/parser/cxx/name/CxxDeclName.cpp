#include "CxxDeclName.h"

CxxDeclName::CxxDeclName(std::wstring name)
	: m_name(std::move(name))
{
}

CxxDeclName::CxxDeclName(std::wstring name, std::vector<std::wstring> templateParameterNames)
	: m_name(std::move(name))
	, m_templateParameterNames(std::move(templateParameterNames))
{
}

NameHierarchy CxxDeclName::toNameHierarchy() const
{
	NameHierarchy ret = getParent() ? getParent()->toNameHierarchy() : NameHierarchy(NAME_DELIMITER_CXX);
	ret.push(std::make_shared<NameElement>(m_name + getTemplateSuffix(m_templateParameterNames)));
	return ret;
}

const std::wstring& CxxDeclName::getName() const
{
	return m_name;
}

const std::vector<std::wstring>& CxxDeclName::getTemplateParameterNames() const
{
	return m_templateParameterNames;
}
