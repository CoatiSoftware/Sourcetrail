#include "data/parser/cxx/name/CxxDeclName.h"

CxxDeclName::CxxDeclName(std::string name, std::vector<std::string> templateParameterNames)
	: m_name(name)
	, m_templateParameterNames(templateParameterNames)
{
}

CxxDeclName::CxxDeclName(
	std::string name,
	std::vector<std::string> templateParameterNames,
	std::shared_ptr<CxxName> parent
)
	: CxxName(parent)
	, m_name(name)
	, m_templateParameterNames(templateParameterNames)
{
}

CxxDeclName::~CxxDeclName()
{
}

NameHierarchy CxxDeclName::toNameHierarchy() const
{
	std::string nameString = m_name;
	if (!m_templateParameterNames.empty())
	{
		nameString += "<";
		for (size_t i = 0; i < m_templateParameterNames.size(); i++)
		{
			if (i != 0)
			{
				nameString += ", ";
			}
			nameString += m_templateParameterNames[i];
		}
		nameString += ">";
	}

	NameHierarchy ret = getParent() ? getParent()->toNameHierarchy(): NameHierarchy(NAME_DELIMITER_CXX);
	ret.push(std::make_shared<NameElement>(nameString));

	return ret;
}

std::string CxxDeclName::getName() const
{
	return m_name;
}

std::vector<std::string> CxxDeclName::getTemplateParameterNames() const
{
	return m_templateParameterNames;
}
