#include "CxxFunctionDeclName.h"

#include <sstream>

CxxFunctionDeclName::CxxFunctionDeclName(
	std::wstring name,
	std::vector<std::wstring> templateParameterNames,
	std::unique_ptr<CxxTypeName> returnTypeName,
	std::vector<std::unique_ptr<CxxTypeName>> parameterTypeNames,
	const bool isConst,
	const bool isStatic
)
	: CxxDeclName(std::move(name), std::move(templateParameterNames))
	, m_returnTypeName(std::move(returnTypeName))
	, m_parameterTypeNames(std::move(parameterTypeNames))
	, m_isConst(isConst)
	, m_isStatic(isStatic)
{
}

NameHierarchy CxxFunctionDeclName::toNameHierarchy() const
{
	std::wstringstream prefix;
	if (m_isStatic)
	{
		prefix << L"static ";
	}
	prefix << m_returnTypeName->toString();

	std::wstringstream postfix;
	postfix << L'(';
	for (size_t i = 0; i < m_parameterTypeNames.size(); i++)
	{
		if (i != 0)
		{
			postfix << L", ";
		}
		postfix << m_parameterTypeNames[i]->toString();
	}
	postfix << L')';
	if (m_isConst)
	{
		postfix << L" const";
	}

	NameHierarchy ret = CxxDeclName::toNameHierarchy();
	ret.back()->setSignature(prefix.str(), postfix.str());
	return ret;
}
