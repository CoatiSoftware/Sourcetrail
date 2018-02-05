#include "data/parser/cxx/name/CxxFunctionDeclName.h"

//CxxFunctionDeclName::CxxFunctionDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxTypeName> returnTypeName,
//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
//	const bool isConst,
//	const bool isStatic
//)
//	: CxxDeclName(name, templateParameterNames)
//	, m_returnTypeName(returnTypeName)
//	, m_parameterTypeNames(parameterTypeNames)
//	, m_isConst(isConst)
//	, m_isStatic(isStatic)
//{
//}

CxxFunctionDeclName::CxxFunctionDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
	const bool isConst,
	const bool isStatic
)
	: CxxDeclName(std::move(name), std::move(templateParameterNames))
	, m_returnTypeName(returnTypeName)
	, m_parameterTypeNames(std::move(parameterTypeNames))
	, m_isConst(isConst)
	, m_isStatic(isStatic)
{
}

//CxxFunctionDeclName::CxxFunctionDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxTypeName> returnTypeName,
//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
//	const bool isConst,
//	const bool isStatic,
//	std::shared_ptr<CxxName> parent
//)
//	: CxxDeclName(name, templateParameterNames, parent)
//	, m_returnTypeName(returnTypeName)
//	, m_parameterTypeNames(parameterTypeNames)
//	, m_isConst(isConst)
//	, m_isStatic(isStatic)
//{
//}

CxxFunctionDeclName::CxxFunctionDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
	const bool isConst,
	const bool isStatic,
	std::shared_ptr<CxxName> parent
)
	: CxxDeclName(std::move(name), std::move(templateParameterNames), parent)
	, m_returnTypeName(returnTypeName)
	, m_parameterTypeNames(std::move(parameterTypeNames))
	, m_isConst(isConst)
	, m_isStatic(isStatic)
{
}

NameHierarchy CxxFunctionDeclName::toNameHierarchy() const
{
	std::wstring signaturePrefix;
	if (m_isStatic)
	{
		signaturePrefix += L"static ";
	}
	signaturePrefix += CxxTypeName::makeUnsolvedIfNull(m_returnTypeName)->toString();

	std::wstring signaturePostfix = L"(";
	for (size_t i = 0; i < m_parameterTypeNames.size(); i++)
	{
		if (i != 0)
		{
			signaturePostfix += L", ";
		}
		signaturePostfix += CxxTypeName::makeUnsolvedIfNull(m_parameterTypeNames[i])->toString();
	}
	signaturePostfix += L")";
	if (m_isConst)
	{
		signaturePostfix += L" const";
	}

	NameHierarchy ret = CxxDeclName::toNameHierarchy();
	std::shared_ptr<NameElement> nameElement = std::make_shared<NameElement>(
		ret.back()->getName(),
		NameElement::Signature(signaturePrefix, signaturePostfix)
	);

	ret.pop();
	ret.push(nameElement);

	return ret;
}
