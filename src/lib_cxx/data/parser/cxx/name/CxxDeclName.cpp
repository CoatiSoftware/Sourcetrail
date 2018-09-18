#include "CxxDeclName.h"

//CxxDeclName::CxxDeclName(const std::wstring& name, const std::vector<std::wstring>& templateParameterNames)
//	: m_name(name)
//	, m_templateParameterNames(templateParameterNames)
//{
//}

CxxDeclName::CxxDeclName(std::wstring&& name, std::vector<std::wstring>&& templateParameterNames)
	: m_name(std::move(name))
	, m_templateParameterNames(std::move(templateParameterNames))
{
}

//CxxDeclName::CxxDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxName> parent
//)
//	: CxxName(parent)
//	, m_name(name)
//	, m_templateParameterNames(templateParameterNames)
//{
//}

CxxDeclName::CxxDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxName> parent
)
	: CxxName(parent)
	, m_name(std::move(name))
	, m_templateParameterNames(std::move(templateParameterNames))
{
}

NameHierarchy CxxDeclName::toNameHierarchy() const
{
	std::wstring nameString = m_name;
	if (!m_templateParameterNames.empty())
	{
		nameString += L"<";
		for (size_t i = 0; i < m_templateParameterNames.size(); i++)
		{
			if (i != 0)
			{
				nameString += L", ";
			}
			nameString += m_templateParameterNames[i];
		}
		nameString += L">";
	}

	NameHierarchy ret = getParent() ? getParent()->toNameHierarchy(): NameHierarchy(NAME_DELIMITER_CXX);
	ret.push(std::make_shared<NameElement>(nameString));

	return ret;
}

std::wstring CxxDeclName::getName() const
{
	return m_name;
}

std::vector<std::wstring> CxxDeclName::getTemplateParameterNames() const
{
	return m_templateParameterNames;
}
