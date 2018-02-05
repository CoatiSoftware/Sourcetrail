#include "data/parser/cxx/name/CxxStaticFunctionDeclName.h"

//CxxStaticFunctionDeclName::CxxStaticFunctionDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxTypeName> returnTypeName,
//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
//	const std::wstring& translationUnitFileName
//)
//	: CxxFunctionDeclName(name, templateParameterNames, returnTypeName, parameterTypeNames, false, true)
//	, m_translationUnitFileName(translationUnitFileName)
//{
//}

CxxStaticFunctionDeclName::CxxStaticFunctionDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
	std::wstring&& translationUnitFileName
)
	: CxxFunctionDeclName(std::move(name), std::move(templateParameterNames), returnTypeName, std::move(parameterTypeNames), false, true)
	, m_translationUnitFileName(std::move(translationUnitFileName))
{
}

//CxxStaticFunctionDeclName::CxxStaticFunctionDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxTypeName> returnTypeName,
//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
//	const std::wstring& translationUnitFileName,
//	std::shared_ptr<CxxName> parent
//)
//	: CxxFunctionDeclName(name, templateParameterNames, returnTypeName, parameterTypeNames, false, true, parent)
//	, m_translationUnitFileName(translationUnitFileName)
//{
//}

CxxStaticFunctionDeclName::CxxStaticFunctionDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
	std::wstring&& translationUnitFileName,
	std::shared_ptr<CxxName> parent
)
	: CxxFunctionDeclName(std::move(name), std::move(templateParameterNames), returnTypeName, std::move(parameterTypeNames), false, true, parent)
	, m_translationUnitFileName(std::move(translationUnitFileName))
{
}

NameHierarchy CxxStaticFunctionDeclName::toNameHierarchy() const
{
	NameHierarchy ret = CxxFunctionDeclName::toNameHierarchy();
	const NameElement::Signature sig = ret.back()->getSignature();

	std::shared_ptr<NameElement> nameElement = std::make_shared<NameElement>(
		ret.back()->getName(),
		NameElement::Signature(sig.getPrefix(), sig.getPostfix() + L" (" + m_translationUnitFileName + L")")
	);

	ret.pop();
	ret.push(nameElement);

	return ret;
}
