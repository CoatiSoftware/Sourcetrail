#include "data/parser/cxx/name/CxxVariableDeclName.h"

//CxxVariableDeclName::CxxVariableDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxTypeName> typeName,
//	bool isStatic
//)
//	: CxxDeclName(name, templateParameterNames)
//	, m_typeName(typeName)
//	, m_isStatic(isStatic)
//{
//}

CxxVariableDeclName::CxxVariableDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxTypeName> typeName,
	bool isStatic
)
	: CxxDeclName(std::move(name), std::move(templateParameterNames))
	, m_typeName(typeName)
	, m_isStatic(isStatic)
{
}

//CxxVariableDeclName::CxxVariableDeclName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateParameterNames,
//	std::shared_ptr<CxxTypeName> typeName,
//	bool isStatic,
//	std::shared_ptr<CxxName> parent
//)
//	: CxxDeclName(name, templateParameterNames, parent)
//	, m_typeName(typeName)
//	, m_isStatic(isStatic)
//{
//}

CxxVariableDeclName::CxxVariableDeclName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateParameterNames,
	std::shared_ptr<CxxTypeName> typeName,
	bool isStatic,
	std::shared_ptr<CxxName> parent
)
	: CxxDeclName(std::move(name), std::move(templateParameterNames), parent)
	, m_typeName(typeName)
	, m_isStatic(isStatic)
{
}

NameHierarchy CxxVariableDeclName::toNameHierarchy() const
{
	std::wstring signaturePrefix;
	if (m_isStatic)
	{
		signaturePrefix += L"static ";
	}
	signaturePrefix += CxxTypeName::makeUnsolvedIfNull(m_typeName)->toString();

	const std::wstring signaturePostfix;

	NameHierarchy ret = CxxDeclName::toNameHierarchy();
	std::shared_ptr<NameElement> nameElement = std::make_shared<NameElement>(
		ret.back()->getName(),
		NameElement::Signature(signaturePrefix, signaturePostfix)
	);

	ret.pop();
	ret.push(nameElement);

	return ret;
}
